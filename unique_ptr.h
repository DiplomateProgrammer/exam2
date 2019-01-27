#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H
#include <memory>
#include <type_traits>
namespace my
{

template <typename T>
struct unique_ptr
{
    struct Deleter
    {
       virtual ~Deleter() = default;
       virtual void delet(T *elem) = 0;
    };
    template <typename func>
    struct Deleter_Impl: Deleter
    {
        Deleter_Impl(func&& f): function(std::move(f)) {}
        Deleter_Impl(func const &f): function(f) {}
        ~Deleter_Impl() override = default;
        void delet(T *elem) override { function(elem); }
        private:
            func function;
    };
    unique_ptr() noexcept: t(nullptr), deleter(nullptr) {}
    unique_ptr(std::nullptr_t): t(nullptr), deleter(nullptr) {}
    void move_impl(unique_ptr &&other) noexcept
    {
        deleter->delet(t);
        if(deleter) { delete deleter; }
        t = std::move(other.t);
        deleter = std::move(other.deleter);
        other.t = nullptr;
        other.deleter = nullptr;
    }
    unique_ptr(unique_ptr &&other) { move_impl(std::move(other)); }
    template <typename func>
    unique_ptr(T *tt, func function): t(tt), deleter(new Deleter_Impl<func>(function)) {}
    ~unique_ptr()
    {
        deleter->delet(t);
        if(deleter) { delete deleter; }
    }
    unique_ptr operator=(unique_ptr &&other)
    {
        if(&other == this) return *this;
        move_impl(std::move(other));
        return *this;
    }
    unique_ptr operator=(std::nullptr_t)
    {
        if(this == nullptr) return *this;
        deleter->delet(t);
        if(deleter) { delete deleter; }
        t = nullptr;
        deleter = nullptr;
        return *this;
    }
    T* release() noexcept
    {
        delete deleter;
        deleter = nullptr;
        T* old = t;
        t = nullptr;
        return old;
    }
    template <typename func>
    void reset(T *tt, func new_deleter)
    {
        T *old = t;
        Deleter *old_deleter = deleter;
        t = tt, deleter = new Deleter_Impl<func>(new_deleter);
        if(old && old_deleter)
        {
            old_deleter->delet(old);
            delete old_deleter;
        }
    }
    void swap(unique_ptr &other) noexcept
    {
       unique_ptr temp(std::move(other));
       other = std::move(*this);
       *this = std::move(temp);
    }
    T* get() const noexcept { return t; }
    T* operator*() const noexcept { return t; }
private:
    T *t;
    Deleter *deleter;
};

template <typename T>
void swap(unique_ptr<T> one, unique_ptr<T> two)
{
    one.swap(two);
}

template <typename T, typename ...Args>
unique_ptr<T> make_unique(Args&& ...args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...), std::default_delete<T>());
}
template <typename T1, typename T2>
bool operator==(unique_ptr<T1> &one, unique_ptr<T2> &two)
{
    return one.get() == two.get();
}

}
#endif // UNIQUE_PTR_H
