#include "bk-List.hpp"

bk::List::List()
    :m_number_of_items(0), 
     m_px_index(reinterpret_cast<list_item_t *>(&m_list_end)), 
     m_list_end()
{
    /* initialize the list_end object */
    this->m_list_end.itemValue = 0xffffU;
    this->m_list_end.pxHead = reinterpret_cast<list_item_t *>(&m_list_end);
    this->m_list_end.pxTail = reinterpret_cast<list_item_t *>(&m_list_end);
}

void bk::List::test(char *newItem)
{

}

void bk::List::insert(void* Item)
{
    bk::List::list_item_t* iter;
    bk::List::list_item_t* newItem = (bk::List::list_item_t *) (Item);

    const uint32_t x_value_of_insertion = newItem->itemValue;

    /* list is empty */
    if (x_value_of_insertion == 0xffffU)
    {
        iter = m_list_end.pxTail;
    }
    else
    {
        /* non empty list, find a position for insertion */
        for (iter = m_list_end.pxHead;
            m_list_end.itemValue <= x_value_of_insertion;
            iter = iter->pxNext)
        {
        }
    }

    /* config newItem object */
    newItem->pxNext = iter->pxNext;
    newItem->pxPrev = iter;
    newItem->pvList = this;

    iter->pxNext = newItem;
    iter->pxNext->pxPrev = newItem;

    m_number_of_items += 1;
}

void bk::List::push_end(bk::List::list_item_t* const newItem)
{
    /* point to the head */
    newItem->pxNext = m_px_index;
    newItem->pxPrev = m_px_index->pxPrev;
    newItem->pvList = this;

    m_number_of_items += 1;
}

