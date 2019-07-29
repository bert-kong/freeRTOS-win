#ifndef __LIST_H__
#define __LIST_H__

#include<stdint.h>

namespace bk
{
    typedef uint32_t tick_type_t;
    class List
    {
    public:
        struct xListItem
        {
            /* constructor/vListInitialiseItem */
            xListItem()
                :itemValue(0xFFFFU), 
				 pxNext(nullptr), pxPrev(nullptr), 
				 pvOwner(nullptr), pvList(nullptr)
            {
            }
            ~xListItem()
            {
                //delete pvOwner;
            }

            /**
             * storage for struct xListItem
             */
            tick_type_t itemValue;
            struct xListItem* pxNext;
            struct xListItem* pxPrev;

            void* pvOwner;

            /* pvContainer */
            List* pvList;
        };
        typedef xListItem list_item_t;

        struct xMiniListItem
        {
            xMiniListItem()
                :pxHead(nullptr), pxTail(nullptr)
            {
            }
            ~xMiniListItem()
            {
            }

            /**
             * storage start
             */
            tick_type_t itemValue;
            list_item_t* pxHead;
            list_item_t* pxTail;
        };

        typedef xMiniListItem mini_list_item_t;

    public:
        /* constructro/vListInitialize */
        List();

        ~List()
        {
        }

        void test(char* item);
  
        /* insert */
        void insert(void* item);

        /* insert_end */
        void push_end(list_item_t* const item);
        //uint32_t remove(list_item_t* const item);


    private:
        /**
         * storage for list
         */
        uint32_t m_number_of_items;
        list_item_t* m_px_index;
        mini_list_item_t m_list_end;
    };


    typedef struct tskTaskControlBlock
    {
        volatile size_t        *pxTopOfStack;    
        bk::List::list_item_t  xStateListItem;    
        List::list_item_t  xEventListItem;        
        uint32_t               uxPriority;            
        size_t                 *pxStack;            
        char                   pcTaskName[ ( 12 ) ]; 
        uint32_t               uxTCBNumber;        
        uint32_t               uxTaskNumber;        
        uint32_t               uxBasePriority;        
        uint32_t               uxMutexesHeld;
        //TaskHookFunction_t     pxTaskTag;
        uint32_t               ulRunTimeCounter;    
        volatile uint32_t      ulNotifiedValue;
        volatile uint8_t       ucNotifyState;
        uint8_t                ucStaticallyAllocated;         
        uint8_t                ucDelayAborted;
    } task_control_block_t;

}

#endif
