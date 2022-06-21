#pragma once
#include<vector>
#include<functional>
#include<memory>



class Slot_ptr
{
public:

private:

};
class Slot
{
public:
    std::shared_ptr<Slot_ptr> slot;
    Slot()
    {
        slot = std::make_shared<Slot_ptr>();
    }
private:
    std::vector<std::weak_ptr<Signal_ptr>> signals;

};


class Signal_ptr
{
public:

private:

};

template<class FuncType>
class Signal
{
public:
    template<class... Args>
    void operator()(Args&&... args)
    {
        for (int i = 0; i < slots.size(); i++)
        {
            
            if (!slots[i].expired())
            {
                (callback[i])(std::forward<Args>(args)...);
            }
            else
            {
                slots.erase(slots.begin() + i);
                callback.erase(callback.begin() + i);
                i--;
            }

        }
    }

    template<class Return,class Type,class... Args>
    void connect(Type* instance, Return(Type::* method)(Args...))
    {
        slots.push_back(instance->slot);
        callback.push_back(bind(instance, method));
    }

    template<class Type>
    void disconnect(Type* instance)
    {
        for (int i = 0; i < slots.size(); i++)
        {
            if ((instance->slot == slots[i].lock()))
            {
                slots.erase(slots.begin() + i);
                callback.erase(callback.begin() + i);
                i--;
            }
        }
    }
    Signal()
    {
        signal = std::make_shared<Signal_ptr>();
    }

    std::shared_ptr<Signal_ptr> signal;

private:
    std::vector<std::weak_ptr<Slot_ptr>> slots;
    std::vector<std::function<FuncType>> callback;
    

    template<class Return, class Type, class... Args>
    std::function<Return(Args...)> bind(Type* instance, Return(Type::* method)(Args...))
    {
        return[=](Args&&... args) -> Return
        {

            return (instance->*method)(std::forward<Args>(args)...);
        };
    }
};


