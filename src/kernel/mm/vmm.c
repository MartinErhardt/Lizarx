/*
 * INFO: Here a Lower-Half Kernel is setup
 */
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <dbg/console.h>
#include <mt/ts.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


/*inline-ASM
 *FIXME move into HAL
 */
#define INVALIDATE_TLB(ADDR) asm volatile("invlpg %0" : : "m" (*(char*)ADDR));

#define ENABLE_PAGING 	uint32_t cr0;\
			asm volatile("mov %%cr0, %0" : "=r" (cr0));\
			cr0 |= (1 << 31);\
			asm volatile("mov %0, %%cr0" : : "r" (cr0));

#define LAST_ADDR 0xffffffff

struct vmm_pagedirentr{
    uint8_t rw_flags; 
    uint8_t reserved :4;
    uintptr_t pagetbl_ptr :20;
}__attribute__((packed));

struct vmm_pagetblentr{
    uint8_t rw_flags; 
    uint8_t reserved :4;
    uintptr_t page_ptr :20;
}__attribute__((packed));

struct vmm_nodepkg_ptr{
    uintptr_t nodepkg_ptr :20;
}__attribute__((packed));


extern const void kernel_start;
extern const void kernel_end;

struct vmm_pagedir{
    struct vmm_pagedirentr pgdir[1024];
}__attribute__((packed, aligned(PAGE_SIZE)));

struct vmm_pagetbl{
    struct vmm_pagetblentr pgtbl[1024];
}__attribute__((packed, aligned(PAGE_SIZE)));

struct vmm_tree_master{
    uint32_t used[32];
    struct vmm_nodepkg_ptr nodepkg[32];
}__attribute__((packed));

struct vmm_tree_nodepkg{
    uint32_t nodepkgentr[1024];
}__attribute__((packed));

static vmm_context startup_context=
    {
	.pd=0x00000000,
	.tr=0x00000000,
    };
bool paging_activated=FALSE;
//--------------------------------------------------------static-function-declarations--------------------------------------------------------------

static void vmm_map_kernel(vmm_context* context);
static int32_t vmm_mark_used_inallcon(struct task*first,uint32_t page);
static int32_t vmm_mark_used(vmm_context*first,uint32_t page);
static bool vmm_is_alloced(vmm_context* context,uint32_t page);
static int32_t vmm_map_inallcon(struct task*first,uintptr_t virt, uintptr_t phys,uint8_t flgs);

vmm_context vmm_init(void)
{
    //int i=0;
    kprintf("[VMM] I: VMM setup...");
    /* Speicherkontext anlegen */
    startup_context = vmm_crcontext();
   
    vmm_set_context(&startup_context);
    
    //while(1){}
    ENABLE_PAGING 
    
    paging_activated=TRUE;
    /*
    for(i=0;i<10;i++){
	kprintf("%x",vmm_is_alloced(&startup_context,i));
    }*/
    
    kprintf("SUCCESS\n");
    //kprintf("paging is 0x%x",paging_activated);
    return startup_context;
}

vmm_context vmm_crcontext(){
    
    vmm_context new_context = {
	.pd=kvmm_malloc(PAGE_SIZE*2),
	.tr=kvmm_malloc(PAGE_SIZE),
    };
    //kprintf("[VMM] I: vmm_crcontext gets physical addr at 0x%x",(uintptr_t)new_context);
    /*
    if((uintptr_t)new_context+PAGE_SIZE >= KERNEL_SPACE){//Is it inside the Kernel?; We are using a Bitmap so 
        kprintf("[VMM] E: Not enough Kenrelspace\n");
    }*/
    
    memset((void*)new_context.pd,0x00000000,PAGE_SIZE*2);// clear the PgDIR to avoid invalid values
    memset((void*)new_context.tr,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values
    //kprintf("hi");
    //kprintf("hi");
    //kprintf("Virt bitmap containig:0x%x\n",*((uint32_t*)((uintptr_t)new_context+PAGE_SIZE/4)));
    vmm_map_kernel(&new_context);
    // kprintf("paging is 0x%x",paging_activated);
    return new_context;
}
/*
 * Getting important when I'm going to implement forks
 * 
vmm_context* vmm_cpycontext(vmm_context* context){
    vmm_context* dst = kvmm_malloc((void*)(uintptr_t)(context));
    memcpy((void*)dst,(void*)context);
    return dst;
}*/
void* kvmm_malloc(size_t size){
    uintptr_t phys=(uintptr_t) (pmm_malloc(size));
    int i;
    struct task* first_task=getldtasks();
    vmm_context* curcontext=getcurcontext();
    if(paging_activated==FALSE){
	//kprintf("[VMM] E: vmm_malloc returns with physical address\n");
	return pmm_malloc(size);
    }
    if((size%PAGE_SIZE)==0){
	size=size/PAGE_SIZE;
    }else{
	size=size/PAGE_SIZE+1;
    }
    
    uintptr_t virt = vmm_find_freemem(curcontext,size,0x00000000,KERNEL_SPACE);
    
    if(virt){
	for(i=0;i<size;i++){
	    if(vmm_map_inallcon(first_task,virt+(i*PAGE_SIZE),phys+(i*PAGE_SIZE),FLGCOMBAT_KERNEL)<0){
		kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_map_page\n");
	    }
	    //kprintf("[VMM] E: kvmm_malloc marking %x size= 0x%x\n",virt/PAGE_SIZE+size,size);
	    vmm_mark_used_inallcon(first_task,virt/PAGE_SIZE+i);
	}
    }else{
	kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_find_freemem\n");
    }
    //kprintf("[VMM] E: alloced at0x%x\n",virt);
     /* size in 4kb(pages) we are adding one page coz we have to allocate pages which have been written on to 1% also*/
     
    return (void*)virt;
}
void* uvmm_malloc(vmm_context* context,size_t size){
    uintptr_t phys=(uintptr_t) (pmm_malloc(size));
    int i;
    if((size%PAGE_SIZE)==0){
	size=size/PAGE_SIZE;
    }else{
	size=size/PAGE_SIZE+1;
    }
    uintptr_t virt = vmm_find_freemem(context,size,KERNEL_SPACE,0xffffffff);
    if(virt){
	for(i=0;i<size;i++){
	    if(vmm_map(context,virt+(i*PAGE_SIZE),phys+(i*PAGE_SIZE),FLGCOMBAT_USER)<0){
		kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_map_page\n");
	    }
	    vmm_mark_used(context,virt/PAGE_SIZE+size);
	}
    }else{
	kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_find_freemem\n");
    }
     /* size in 4kb(pages) we are adding one page coz we have to allocate pages which have been written on to 1% also*/
     
    return (void*)virt;
}
uintptr_t vmm_find_freemem(vmm_context* context,uint32_t size, uintptr_t from,uintptr_t to){
    uintptr_t virt=0x0;// avoid unitialized Pointer Bug
    int i,j;
    for (i = from/PAGE_SIZE; i < to/PAGE_SIZE-(size-1); i++) {// find and map free 
      
mark:
	
	for(j=0;j<size;j++){
	    //kprintf("in loop: %d\n",i);
	    if(vmm_is_alloced((void*)((uintptr_t)(context)),i+j)==TRUE){
	      /*
		if(i%1024==1023){
		    context->tr->used[i/32768]&=(1<<((i/1024)%32));
		}*/
		//kprintf("[VMM] i=%x j=%d\n",i,j);
		i++;
		goto mark;
	    } 
	    else if(j==size-1){
		virt = i*PAGE_SIZE;
		//kprintf("[VMM] i=%d j=%d\n",i,j);
		//kprintf("[VMM] E: vmm_find_freemem found free space in addressspace at 0x%x\n",virt);
		//kprintf("found free space at 0x%x\n",virt);
		goto out;
	    }
	}
    }
out:
    
    if(virt ==0x0){// nothing was found
       kprintf("[VMM] E: vmm_find_freemem couldn't get free space in addressspace\n");
       return 0x0;
    }else{
      return virt;
    }
}
int32_t vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs)
{
    int j,k;
    uintptr_t page =(uintptr_t) ptr/PAGE_SIZE;
    uintptr_t phys=(uintptr_t) pmm_malloc(size);
    size=size/PAGE_SIZE+1;
    /*
    kprintf(" 0xffff is %d",vmm_is_alloced(virt_bitmap,0xffff));
    kprintf(" bitmap at %d",(uintptr_t)virt_bitmap);
    */
    for(j=0;j<size;j++) {

	    if (vmm_is_alloced(context,page+j)==TRUE) {
		kprintf("[VMM] E: vmm_realloc says: page 0x%x000 alloiced\n",page +j);
		return -1;
	    } else if(j==size-1){
		for(k =0;k<size;k++){
		    if(vmm_map(context,(page+k)*PAGE_SIZE,phys,flgs)<0){
			kprintf("[VMM] E: vmm_realloc called vmm_map_page and got error\n");
			return -1;
		    }
		}
		return 0;
	}
    }
    kprintf("[VMM] E: vmm_realloc couldn't get free space in addressspace\n");
    return -1;
}
/*
 * FIXME Map tmp at 0x1000
 */
int32_t vmm_free(vmm_context* context,void* page)//FIXME No Overflow check
{
    uint32_t page_index = (uintptr_t)(page) / PAGE_SIZE;
    struct vmm_pagedirentr *pdirentr = &context->pd->pgdir[page_index/1024];
    struct vmm_pagetbl *pgtbl = (struct vmm_pagetbl *) ((uintptr_t)(pdirentr->pagetbl_ptr*PAGE_SIZE));
    struct vmm_pagetblentr *pgtblentr=&pgtbl->pgtbl[page_index%1024];
    if((pgtblentr->page_ptr==0x00000000)||(pdirentr->pagetbl_ptr==0x00000000)){
	kprintf("[VMM] I: vmm_free can't free page- page not mapped\n");
	return -1;
    }
    memset(&pgtblentr,0x00000000,4);
    return 0;
}
/*
 * DONE Write copyin/out for user-kernel data exchange
 *
 */

void* cpyout(vmm_context* context,void* src,size_t siz){
    void* dst =uvmm_malloc((void*)((uintptr_t)(context)),siz);
    memcpy(dst,src,siz);
    return dst;
}
void* cpyin(void* src,size_t siz){
    void* dst =kvmm_malloc(siz);
    memcpy(dst,src,siz);
    return dst;
}
int32_t vmm_map_inallcon(struct task*first,uintptr_t virt, uintptr_t phys,uint8_t flgs){
    struct task* curtask =first;
    struct task* next = NULL;
    if(curtask!=NULL){
      next = curtask->next;
      while(next==first){
	if(vmm_map(curtask->context, virt,phys,flgs)){
	    kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
	    return -1;
	}
	curtask=next;
	next=curtask->next;
      }
    }
    if(startup_context.pd){
	if(vmm_map(&startup_context,virt,phys,flgs)){
	    kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
	    return -1;
	}
    }
    return 0;
}
int vmm_map(vmm_context* context, uintptr_t virt, uintptr_t phys,uint8_t flgs){
    struct vmm_pagetbl* page_table=NULL;
    
    uint32_t page =virt/PAGE_SIZE;
    uint32_t pd_index = page / 1024;
    uint32_t pt_index = page % 1024;
    vmm_context* curcontext=getcurcontext();
    // Wir brauchen 4k-Alignment 
    if (((virt % PAGE_SIZE)!=0) || ((phys % PAGE_SIZE)!= 0)) {
	kprintf("[VMM] E: vmm_map_page has no 4k alignment(virt: 0x%x,phys: 0x%x)\n",virt,phys);
	return -1;
    }
    //kprintf("paging is 0x%x",paging_activated);
    
    
    struct vmm_pagetblentr new_pgtblentr ={
	.rw_flags = flgs,
	.reserved = 0x0,
	.page_ptr =phys/PAGE_SIZE
    };
    /*
     * -----------------------------------------------------find-or-create-pagetable---------------------------------------------------------------
     */
    if (context->pd->pgdir[pd_index].rw_flags& FLG_IN_MEM) 
    {
        /* Page Table ist schon vorhanden */
	page_table = (void*)(context->pd->pgdir[pd_index].pagetbl_ptr*PAGE_SIZE);
        
	
	
	if((paging_activated)&&(virt != TMP_PAGEBUF))// if paging is activated TMP_PAGEBUF will be allways mapped
	{
	      //kprintf("map pgtbl at 0x%x",(uintptr_t)(getcurcontext()));
	      vmm_map(curcontext,TMP_PAGEBUF,(uintptr_t)page_table,FLGCOMBAT_KERNEL);
	      //kprintf("__fin");
	      page_table =(struct vmm_pagetbl *)TMP_PAGEBUF;
	}
	else if(virt==TMP_PAGEBUF){
	      page_table =(struct vmm_pagetbl *)((uintptr_t)(context->pd)+PAGE_SIZE);
	}
    } 
    else 
    {
	
        /* Neue Page Table muss angelegt werden */
	if(pd_index==0){
		page_table =(struct vmm_pagetbl *)((uintptr_t)(context->pd)+PAGE_SIZE);
	}
	else
	{
		page_table = (struct vmm_pagetbl*)(pmm_malloc(PAGE_SIZE));
	}
	
	
	if(page_table==NULL)
	{
	    kprintf("[VMM] E: vmm_map_page doesn't found a Page Table\n");
	    return -1;
	}
	
	//kprintf("her");
	
	/*set Pagedir entry*/
	struct vmm_pagedirentr new_pgdirentr ={
	    .rw_flags = flgs,
	    .reserved=0x0,
	    .pagetbl_ptr =(uintptr_t)(page_table)/PAGE_SIZE
	};
	
        context->pd->pgdir[pd_index] =new_pgdirentr;
	//kprintf("pgenb at 0x%x, virt at0x%x",(uintptr_t)&paging_activated,virt);
	if((paging_activated)&&(virt!=TMP_PAGEBUF))
	{// if paging is activated TMP_PAGEBUF will be allways mapped
		vmm_map(curcontext,TMP_PAGEBUF,(uintptr_t)page_table,FLGCOMBAT_KERNEL);
		page_table =(struct vmm_pagetbl*)TMP_PAGEBUF;
		//kprintf("hi5");
	}
	memset(page_table, 0x00000000, PAGE_SIZE);
    } 
    //kprintf("[VMM] I: vmm_map_page using Context at 0x%x\n",(uintptr_t)context);
    //set pagetableentry 
    //kprintf("pgttbl at 0x%x",(uintptr_t)&page_table->pgtbl[pt_index]);
    page_table->pgtbl[pt_index] =new_pgtblentr;
    
    INVALIDATE_TLB(virt)
    
    if((paging_activated)&&(virt!=TMP_PAGEBUF))
    {
	vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	//kprintf("context at 0x%x",(void*)((uintptr_t)(getcurcontext())));
    }
    //kprintf("context at 0x%x",(void*)((uintptr_t)(getcurcontext())));
    //kprintf("map");
    //kprintf("mapped phys address 0x%x to virt 0x%x\n",phys,virt);
    return 0;
}
bool vmm_is_alloced(vmm_context* context,uint32_t page)//FIXME No Overflow check
{
    uint32_t master_ind = page/1024;
    uint32_t node_ind=page%1024;
    uint32_t inner_nodepkgoff=(master_ind%32)*1024;
    struct vmm_tree_master*tree = context->tr;
    struct vmm_tree_nodepkg* nodes = (void*)(tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
    //kprintf("is alloced,%d,at nodeentr 0x%xat node0x%x\n",page,(uintptr_t)&nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32],(uintptr_t)&nodes);
    
    /*if(tree->used[master_ind])
    {
	kprintf("0x%x is alloced1 used at 0x%x",page*PAGE_SIZE,(uintptr_t)&tree->used[master_ind]);
	return TRUE;
    }*/
    if(nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]&(1<<((node_ind+inner_nodepkgoff)%32)))
    {
	return TRUE;
    }
    else{
	return FALSE;
    }
}
int32_t vmm_mark_used(vmm_context* context,uint32_t page){
    uint32_t master_ind = page/1024;
    uint32_t node_ind=page%1024;
    uint32_t inner_nodepkgoff=(master_ind%32)*1024;
    struct vmm_tree_master*tree = context->tr;
    struct vmm_tree_nodepkg* nodes = (void*)(tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
    if(page>=KERNEL_SPACE/PAGE_SIZE){
	//kprintf("page 0x%x",page);
    }
    /*
    if(tree->used[master_ind])
    {
	return 0;
    }*/
    if(nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]&(1<<((node_ind+inner_nodepkgoff)%32)))
    {
	//kprintf("0x%x is alloced",page*PAGE_SIZE);
	return 0;
    }
    else{
	//kprintf("node shift at 0x%x,inner_nodeoff=0x%x,node ind=0x%x,master_ind=0x%x\n",(uintptr_t)(node_ind+inner_nodepkgoff)%32,(uintptr_t)nodes,inner_nodepkgoff,node_ind,master_ind);
	nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]|=(1<<((node_ind+inner_nodepkgoff)%32));
	return 0;
    }
}
int32_t vmm_mark_used_inallcon(struct task*first,uint32_t page){
    struct task* curtask =first;
    struct task* next = NULL;
    //kprintf("[VMM] E: vmm_mark_usedinallcon marking %d\n",page);
    if(curtask!=NULL){
      next = curtask->next;
      while(next==first){
	if(vmm_mark_used(curtask->context,page)){
	    kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
	    return -1;
	}
	curtask=next;
	next=curtask->next;
      }
    }
    if(startup_context.pd){
      
	if(vmm_mark_used(&startup_context,page)){
	    kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
	    return -1;
	}
    }
    return 0;
}
void vmm_set_context(vmm_context* context)
{
    asm volatile("mov %0, %%cr3" : : "r" (context->pd));
    //INVALIDATE_TLB(virt)
}

static void vmm_map_kernel(vmm_context* context){
    int i=0;
    vmm_context* 	curcontext=getcurcontext();
    struct task*		curtask=getcurtask();
    uintptr_t node_virt=0x0;
    uintptr_t node_phys =0x0;
    uintptr_t phys =0x0;
    //     Die ersten 4 MB an dieselbe physische wie virtuelle Adresse mappen 
    /*
    for (i = 0; i < BITMAP_SIZE/PAGE_SIZE/32; i += 0x1000) {
        vmm_map_page(context, i, i,FLGCOMBAT_KERNEL);
    }*/
    
    //kprintf("[VMM] I : vmmm_mapkernel displays addr_space:\n");
    //  map everything within Kernel space 
    //kprintf("map kernel");
    
    if(paging_activated)
    {
	for(i=0;i<32;i++){
		node_virt=vmm_find_freemem(curcontext,1,0x0,KERNEL_SPACE/PAGE_SIZE);
		node_phys =(uintptr_t)pmm_malloc(PAGE_SIZE);
		context->tr->nodepkg[i].nodepkg_ptr=node_virt/PAGE_SIZE;
		vmm_map(curcontext,node_virt,node_phys,FLGCOMBAT_KERNEL);
		memset((void*)node_virt,0x00000000,PAGE_SIZE);
		if((uintptr_t)curcontext)
		{
			vmm_mark_used_inallcon(curtask,context->tr->nodepkg[i].nodepkg_ptr/PAGE_SIZE);
			vmm_mark_used_inallcon(curtask,node_virt/PAGE_SIZE);
		}
		
		if((uintptr_t)startup_context.pd)
		{
			vmm_mark_used(&startup_context,context->tr->nodepkg[i].nodepkg_ptr/PAGE_SIZE);
			vmm_mark_used(&startup_context,node_virt/PAGE_SIZE);
		}
		//kprintf("tree%d node virt = 0x%x",i,node_virt);
	}
	for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++) 
	{
		//phys=virt_to_phys(curcontext,i*PAGE_SIZE);
		if(vmm_is_alloced(curcontext,i))
		{
			
			phys=virt_to_phys(curcontext,i*PAGE_SIZE);
			
			if(vmm_map(context,i*PAGE_SIZE,phys,FLGCOMBAT_KERNEL)<0)
			{
				kprintf("erro"); 
			}
			vmm_mark_used(context,i);
		}
	}
		//kprintf("first node at 0x%xto 0x%x ",
		//context->tr->nodepkg[i].nodepkg_ptr,
		//virt_to_phys(context,context->tr->nodepkg[i].nodepkg_ptr));
    }
    else
    {
	for(i=0;i<32;i++){
		node_phys =(uintptr_t)pmm_malloc(PAGE_SIZE);
		//kprintf("0x%x",node_phys);
		context->tr->nodepkg[i].nodepkg_ptr=node_phys/PAGE_SIZE;
		memset((void*)node_phys,0x00000000,PAGE_SIZE);
	}
	for(i=0;i<32;i++){
		node_phys=context->tr->nodepkg[i].nodepkg_ptr*PAGE_SIZE;
		pmm_mark_used((uint32_t*)node_phys);
	}
	for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++) 
	{
		if(pmm_is_alloced(i))
		{
			//kprintf("hi");
			if(vmm_map(context,i*PAGE_SIZE,i*PAGE_SIZE,FLGCOMBAT_KERNEL)<0)
			{
				kprintf("err");
			}
			vmm_mark_used(context,i);
		}
	}
    }
    //return -1;
    //kprintf("fin");
}
uintptr_t phys_to_virt(vmm_context* context,uintptr_t phys){
    
    return 0;
}
uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt){
    uint32_t page =virt/PAGE_SIZE;
    uint32_t phys =0;
    uint32_t pd_index = page / 1024;
    uint32_t pt_index = page % 1024;
    vmm_context* 	curcontext=getcurcontext();
    struct vmm_pagedirentr *pdirentr = &(context->pd->pgdir[pd_index]);
    //kprintf("pdind %d virt: 0x%x pt_index: %d\n",pd_index,virt,pt_index);
    struct vmm_pagetbl *pgtbl = (struct vmm_pagetbl *) ((uintptr_t)(pdirentr->pagetbl_ptr*PAGE_SIZE));
    
    //kprintf("pg at 0x%xdflsafsdlkfj\n",pdirentr->pagetbl_ptr);
    if((paging_activated)&&(virt!=TMP_PAGEBUF))
    {
	vmm_map(curcontext,TMP_PAGEBUF,(uintptr_t)pgtbl,FLGCOMBAT_KERNEL);
	//kprintf("pd at 0x%x",(uintptr_t)pgtbl);
	pgtbl=(struct vmm_pagetbl *)0x1000;
    }
    struct vmm_pagetblentr *pgtblentr=&pgtbl->pgtbl[pt_index];
    //kprintf("pt at 0x%x\n",pgtblentr->page_ptr);
    //while(1){}
    if((pgtblentr->page_ptr==0x00000000)||(pdirentr->pagetbl_ptr==0x00000000)){
	//kprintf("[VMM] I: virt_to_phys can't calc virt addr pge at 0x%x virt = 0x%x\n",(uintptr_t)pgtblentr,virt);
	return 0x0;
    }
    phys=pgtblentr->page_ptr*PAGE_SIZE;
    if((paging_activated)&&(virt!=TMP_PAGEBUF)){
	//kprintf("context at 0x%x",(void*)((uintptr_t)(getcurcontext())));
	vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
    }
    return phys;
}

vmm_context* get_startupcontext(){
    return &startup_context;
}
void set_startupcontext(vmm_context* context){
    startup_context =*context;
}