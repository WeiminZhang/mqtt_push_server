#include <stdlib.h>

int timer_init(struct util_timer_list *list)
{
    list = malloc(sizeof(struct util_timer_list));
    if(!list)
    {
        return MQTT_ERR_NO_MEM;
    }

    list->head = NULL;
    list->tail = NULL;
}

int add_timer(struct util_timer_list *list, struct util_timer *timer)
{
    if(!list || !timer)
    {
        return MQTT_ERR_NULL;
    }
    if(list->head == NULL)
    {
        list->head = list->tail = timer;
        return MQTT_ERR_SUCCESS;
    }

    if(timer->expire < list->head->expire)
    {
        timer->next = list->head;
        list->head->prev = timer;
        list->head = timer;
        return MQTT_ERR_SUCCESS;
    }
    

    return add_timer_after(list, timer, list->head);
}
int add_timer_after(struct util_timer_list *list, struct util_timer *timer, struct util_timer *tar)
{
    struct util_timer *tmp = tar->next;
    while(tmp != NULL || timer->expire > tmp->expire) tmp = tmp->next;
    if(tmp != NULL)
    {
        timer->prev = tmp->prev;
        tmp->prev->next = timer;
        timer->next = tmp;
        tmp->prev = timer;
    }else{
        tmp->next = timer;
        timer->prev = tmp;
        list->tail = timer;
    }

    return MQTT_ERR_SUCCESS;
}
int adjust_timer(struct util_timer_list *list, struct util_timer *timer)
{
    if(!list || !timer) return MQTT_ERR_NULL;

    struct util_timer *tmp;
    
    if(timer == list->tail) return MQTT_ERR_SUCCESS;

    if(timer->expire < timer->next->expire) return MQTT_ERR_SUCCESS;

    if(timer == list->head)
    {
        list->head->next->prev = NULL;
        list->head = list->head->next;
        timer->next = NULL;
        return add_timer(list, timer);
    }else{
        timer->next->prev =timer->prev;
        timer->prev->next = timer->next;
        tmp = timer->next; 
        timer->prev = timer->next = NULL;
        return add_timer_after(list, timer, tmp)
    }
}

void timer_tick(struct util_timer_list *list)
{
    if(!list || !list->head) return;

    timer_t cur = time(NULL);
    struct util_timer *tmp = list->head;
    while(tmp)
    {
        if(cur < tmp->expire)
        {
            break;
        }
        tmp->cb_func(tmp->user_data);
        list->head = tmp->next;
        if(list->head)
        {
            head->prev = NULL;
        }
        free(tmp);
        tmp = list->head;
    }
}