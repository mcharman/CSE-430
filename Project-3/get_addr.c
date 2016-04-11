/*		*	*	*	*	*	*	*	*	*	*	*	*
*
*		Group Names:		Mark Harman	
*							Wesley Springer
*							Chandrakanth Reddy
*		Assignment:			Project 3 Part I
*		Date:				4/7/16
*		Description:		Given a PID and virtual address we search all processes
*							in the page table for a matching reference and return the
*							physical memory address where the PID is stored
*	*	*	*	*	*	*	*	*	*	*	*	*	*/

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/tty.h>
#include <linux/linkage.h>
#include <linux/highmem.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/rmap.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/hugetlb.h>

#include <asm/page.h>
#include <asm/pgtable_types.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>

asmlinkage unsigned long get_addr(int pid, unsigned long address, int print)
{
    pgd_t* pgd;
    pud_t* pud;
    pmd_t* pmd;
    pte_t *ptep, pte;
    spinlock_t* ptl;

    struct task_struct* task;

    for_each_process(task)
    {
    	if(task->pid == pid)
    	{
    		//the following code is used for walking the page table
    		//it returns the struct page used by the PTE at address
    		// in mm's page table, it is a modified version of follow_page()
    		//the code was taken from www.kernel.org
    		pgd = pgd_offset(task->mm, address);
    		if (pgd_none(*pgd) || pgd_bad(*pgd))
				return -1;
			
    		pud = pud_offset(pgd, address);
    		if (pud_none(*pud) || pud_bad(*pud))
				return -1;
			
    		pmd = pmd_offset(pud, address);
    		if (pmd_none(*pmd) || pmd_bad(*pmd))
				return -1;
			
    		ptep = pte_offset_map_lock(task->mm, pmd, address, &ptl);
    		pte = *ptep;
    		pte_unmap_unlock(ptep, ptl);

    		if(pte_present(pte))
    		{
    			if(print == 0)
    				return pte_pfn(pte);
    			else
    			{
    				unsigned long npfn = pte_pfn(pte);
    				unsigned long offset = (address & 0x00000FFF);
    				unsigned long npfn_shift = (npfn << 12);
    				unsigned long paddr = (npfn_shift | offset);
    				return paddr;
    			}
    		}
    		else
    		{
    			if(!pte_none(pte))
    			{
    				swp_entry_t swp;
    				swp = pte_to_swp_entry(pte);
    				if(print == 0)
    					return swp_offset(swp);
    				else
    					return swp_type(swp);
    			}
    			else
    				return -1;
    		}

    	}
    }
    return -1;
}
