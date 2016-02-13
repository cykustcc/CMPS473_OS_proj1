
/**********************************************************************

   File          : cse473-p1-mfu.c

   Description   : This is most frequently used replacement algorithm
                   (see .h for applications)

   By            : Trent Jaeger, Yuquan Shan

***********************************************************************/
/**********************************************************************
Copyright (c) 2016 The Pennsylvania State University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of The Pennsylvania State University nor the names of its contributors may be used to endorse or promote products derived from this softwiare without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <sched.h>

/* Project Include Files */
#include "cse473-p1.h"

/* Definitions */

/* second chance list */

typedef struct mfu_entry{  
  int pid;
  ptentry_t *ptentry;
  struct mfu_entry *next;
  struct mfu_entry *prev;
} mfu_entry_t;

typedef struct mfu{
  mfu_entry_t *first;
} mfu_t;

mfu_t *page_list;

/**********************************************************************

    Function    : init_mfu
    Description : initialize mfu list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_mfu( FILE *fp )
{
  printf("initiate mfu...\n");
  page_list = (mfu_t *)malloc(sizeof(mfu_t));
  page_list->first = NULL;
  return 0;
}

/**********************************************************************

    Function    : update_mfu
    Description : create container for the newly allocated frame (and 
                  associated page), and insert it to the end (with respect
                  to page_list->first) of page list 
    Inputs      : pid - process id
                  f - frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/
void print_mfu(){
  mfu_entry_t *mfu_ptr=page_list->first;
  int first_access=1;
  printf("----");
  while(mfu_ptr->ptentry->frame!=page_list->first->ptentry->frame||first_access){
    first_access=0;
    printf("frame(%d)\t",mfu_ptr->ptentry->frame);
    mfu_ptr=mfu_ptr->next;
  }
  printf("----\n");
}

/**********************************************************************

    Function    : replace_mfu
    Description : choose victim based on mfu algorithm, take the frame 
                  associated the page with the largest count as victim 
    Inputs      : pid - process id of victim frame 
                  victim - frame assigned from fifo -- to be replaced
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int replace_mfu( int *pid, frame_t **victim )
{
  printf("replace_mfu: pid=%d\n",*pid);
  // replace the most frequently used page.
  /* Task 3 */
  if (page_list->first==NULL)
  {
    exit(-1);
  }else{
    /* return info on victim */
    *pid=page_list->first->pid;
    *victim=&physical_mem[page_list->first->ptentry->frame];
    int highest_count=page_list->first->ptentry->ct; // record the use count of the most frequently used frame
    mfu_entry_t *mfu_ptr=page_list->first->next; // pointer to the list to iterate through it
    printf("hahaha\n");
    print_mfu();
    while(mfu_ptr!=page_list->first){
      if(mfu_ptr->ptentry->ct>highest_count)
      {
        highest_count=mfu_ptr->ptentry->ct;
        *pid=mfu_ptr->pid;
        *victim=&physical_mem[mfu_ptr->ptentry->frame];
      }
      mfu_ptr=page_list->first->next;
    }
    /* remove from list */
    mfu_ptr->prev->next=mfu_ptr->next;
    mfu_ptr->next->prev=mfu_ptr->prev;
    free(mfu_ptr);
  }
  return 0;
}



/**********************************************************************

    Function    : update_mfu
    Description : create container for the newly allocated frame (and 
                  associated page), and insert it to the end (with respect
                  to page_list->first) of page list 
    Inputs      : pid - process id
                  f - frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_mfu( int pid, frame_t *f )
{
  printf("update_mfu: pid=%d, frame=%d\n",pid,f->number);
  /* Task 3 */
  ptentry_t* pid_s_pt=processes[pid].pagetable;
  mfu_entry_t *list_entry=( mfu_entry_t *)malloc(sizeof(mfu_entry_t));
  list_entry->ptentry = pid_s_pt;
  list_entry->pid = pid;
  if(page_list->first==NULL)
  {
      list_entry->prev=list_entry;
      list_entry->next=list_entry;
      page_list->first=list_entry;
  }else{
      list_entry->prev=page_list->first->prev;
      list_entry->next=page_list->first;
      page_list->first->prev->next=list_entry;
      page_list->first->prev=list_entry;
  }
  print_mfu();
  return 0;
}
