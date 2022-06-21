#pragma once
#include<vector>
#include<functional>
#include<memory>

class Slot_ptr;

class Signal_p 
{
public:
    virtual ~Signal_p() {}
    virtual void disconnect(std::shared_ptr<Slot_ptr>) = 0;
};


class Slot_ptr:public std::enable_shared_from_this<Slot_ptr>
{
public:
    void set(std::shared_ptr<Signal_p> signal )
    {
        for (int i = 0; i < signals.size(); i++)
        {
            if (signals[i].lock() == signal)
            {
                signal_nums[i]++;
                return;
            }
        }
        signals.push_back(signal);
        signal_nums.push_back(1);
    }
    void un_set(std::shared_ptr<Signal_p> signal)
    {
        for (int i = 0; i < signals.size();i++)
        {
            if (signals[i].lock() == signal)
            {
                signal_nums[i]--;
                if (signal_nums[i] == 0)
                {
                    signals.erase(signals.begin() + i);
                    signal_nums.erase(signal_nums.begin() + i);
                    i--;
                }
                return;
            }
        }
    }
    std::vector<std::weak_ptr<Signal_p>> signals; 
    std::vector<int> signal_nums;
};

class Slot
{
public:
    std::shared_ptr<Slot_ptr> slot;
    Slot()
    {
        slot = std::make_shared<Slot_ptr>();
    }
    ~Slot()
    {
        for (int i = 0; i < slot->signals.size(); i++)
        {
            slot->signals[i].lock()->disconnect(slot);
        }
    }
};





template<class FuncType>
class Signal_ptr:public Signal_p, public std::enable_shared_from_this<Signal_ptr<FuncType>>
{
public:
    template<class... Args>
    void call(Args&&... args)
    {
        for (int i = 0; i < slots.size(); i++)
        {
            (callback[i])(std::forward<Args>(args)...);
        }
    }
    template<class Return, class Type, class... Args>
    void connect(Type* instance, Return(Type::* method)(Args...))
    {
        slots.push_back(instance->slot);
        callback.push_back(bind(instance, method));
        callback_ptr.push_back(reinterpret_cast<void *>(&method));
        instance->slot->set(this->shared_from_this());
    }

    template<class Return, class Type, class... Args>
    void disconnect(Type* instance, Return(Type::* method)(Args...))
    {
        for (int i = 0; i < slots.size(); i++)
        {
            if ((instance->slot == slots[i].lock())&&(callback_ptr[i]== reinterpret_cast<void*>(&method)))
            {
                slots.erase(slots.begin() + i);
                callback.erase(callback.begin() + i);
                callback_ptr.erase(callback_ptr.begin() + i);
                instance->slot->un_set(this->shared_from_this());
                i--;
            }
        }
    }
    void disconnect(std::shared_ptr<Slot_ptr> slot) override//slot析构调用此函数，让所有和该slot关联的signal中的对应slot失效
    {
        for (int i = 0; i < slots.size(); i++)
        {
            if ((slot == slots[i].lock()))
            {
                slots.erase(slots.begin() + i);
                callback.erase(callback.begin() + i);
                callback_ptr.erase(callback_ptr.begin() + i);
                i--;
            }
        }
    }
    std::vector<std::weak_ptr<Slot_ptr>> slots;
    std::vector<std::function<FuncType>> callback;
    std::vector<void*> callback_ptr;

    template<class Return, class Type, class... Args>
    std::function<Return(Args...)> bind(Type* instance, Return(Type::* method)(Args...))
    {
        return[=](Args&&... args) -> Return
        {
            return (instance->*method)(std::forward<Args>(args)...);
        };
    }

};

template<class FuncType>
class Signal
{
public:
    Signal()
    {
        signal = std::make_shared<Signal_ptr<FuncType>>();
    }
    ~Signal()
    {
        for (int i = 0; i < signal->slots.size(); i++)
        {
            signal->slots[i].lock()->un_set(signal);
        }
    }
    std::shared_ptr<Signal_ptr<FuncType>> signal;

    template<class... Args>
    void operator()(Args&&... args)
    {
        signal->call(std::forward<Args>(args)...);
    }

    template<class Return, class Type, class... Args>
    void connect(Type* instance, Return(Type::* method)(Args...))
    {
        signal->connect(instance, method);
    }

    template<class Return, class Type, class... Args>
    void disconnect(Type* instance, Return(Type::* method)(Args...))
    {
        signal->disconnect(instance,method);
    }
};

template<class FuncType, class Return, class Type, class... Args>
void connect(Signal<FuncType> &signal, Type* instance, Return(Type::* method)(Args...))
{
    signal.connect(instance, method);
}

template<class FuncType, class Return, class Type, class... Args>
void disconnect(Signal<FuncType> &signal, Type* instance, Return(Type::* method)(Args...))
{
    signal.disconnect(instance, method);
}