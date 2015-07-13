#pragma once

template<typename T>
inline void intrusive_ptr_add_ref(T* expr)
{
    ++expr->references;
}

template<typename T>
inline void intrusive_ptr_release(T* expr)
{
    if(--expr->references == 0)
        delete expr;
}

