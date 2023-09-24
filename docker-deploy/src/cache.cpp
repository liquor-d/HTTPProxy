#include "cache.hpp"

Cache::Cache() :head(NULL){
    tail = head;
    num_cache = 0;
}

HttpResponse Cache::find_cache(std::string find_start_line){
    Node *it = head;
    if(it == NULL){
        return HttpResponse();
    }
    while(it != NULL){
        if(it->start_line == find_start_line){
            return it->response;
        }
        it = it->next;
    }
    return HttpResponse();
}

void Cache::add_cache(std::string start_line, HttpResponse response){
    num_cache +=1;
    Node *newNode = new Node(start_line, response);
    if(head == NULL){
        head = newNode;
        tail = newNode;
    }
    else if (head == tail){
        head = newNode;
        newNode->next = tail;
        tail->prev = newNode;
    }
    else{
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
}

void Cache::remove_cache(std::string find_start_line){
    num_cache -=1;
    if(head == NULL){
        return;
    }
    else if(head == tail && head->start_line == find_start_line){
        head = NULL;
        tail = NULL;
    }
    else if(head->start_line == find_start_line){
        head->next->prev = NULL;
        head = head->next;
    }
    else if(tail->start_line == find_start_line){
        tail->prev->next = NULL;
        tail = tail->prev;
    }
    else{
        Node *it = head;
        while(it != NULL){
            if(it->start_line == find_start_line){
                it->prev->next = it->next;
                it->next->prev = it->prev;
                break;
            }
            it = it->next;
        }
    }

}

bool Cache::check_cache(HttpResponse &response, std::string req_start_line){
    if(response.get_msg().find("HTTP/1.1 200 OK") == std::string::npos){
        return false;
    }
    else if(response.is_no_store()){
        return false;
    }
    // else if(response.isNoCache()){
    //     return;
    // }
    return true;

}

void Cache::print(){
    std::cout << "Current cache:" << num_cache << std::endl;
    std::cout << std::endl;
    Node *it = head;
    while(it != NULL){
        std::cout << it->start_line << " : " << it->response.get_msg() << std::endl;
        it = it->next;
    }
    return;
}