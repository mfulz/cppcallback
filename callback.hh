#include <cstring>

class NCallbackBase {
public:
    typedef void (NCallbackBase::*_MemFunc)();
    typedef void (*_Func)();

    NCallbackBase()
        : func(nullptr)
        , callee(nullptr)
    {
    }

    NCallbackBase(const void* c, const void* f, size_t sz)
    {
        if (c) {
            callee = (void*)c;
            memcpy(memFunc, f, sz);
        }
        else {
            func = f;
        }
    }

    operator int() const { return func || callee; }

    class DummyInit {
    };

    union {
        const void* func;
        char memFunc[sizeof(_MemFunc)];
    };

    void* callee;
};

template <typename RetType, typename... Params>
class NCallback : protected NCallbackBase {
public:
    NCallback(DummyInit* = nullptr) {}

    RetType operator()(Params... params) const
    {
        return callback(*this, params...);
    }

    using NCallbackBase::operator int;

protected:
    typedef RetType (*Callback)(const NCallbackBase&, Params...);

    NCallback(Callback _callback, const void* _instance, const void* _fct, size_t _sz)
        : NCallbackBase(_instance, _fct, _sz)
        , callback(_callback)
    {
    }

private:
    Callback callback;
};

template <typename RetType, typename Func, typename... Params>
class FunctionTranslator : public NCallback<RetType, Params...> {
public:
    FunctionTranslator(Func fct)
        : NCallback<RetType, Params...>(callback, nullptr, (const void*)fct, 0)
    {
    }

    static RetType callback(const NCallbackBase& ftor, Params... params)
    {
        return (Func(ftor.func))(params...);
    }
};

template <typename RetType, typename Callee, typename MemFunc, typename... Params>
class MemberTranslator : public NCallback<RetType, Params...> {
public:
    MemberTranslator(Callee& _instance, const MemFunc& _method)
        : NCallback<RetType, Params...>(callback, &_instance, &_method, sizeof(MemFunc))
    {
    }

    static RetType callback(const NCallbackBase& ftor, Params... params)
    {
        Callee* callee = (Callee*)ftor.callee;
        MemFunc& memFunc(*(MemFunc*)(void*)(ftor.memFunc));
        return (callee->*memFunc)(params...);
    }
};

template <typename RetType, typename FuncType, typename... Params, typename... FuncParams>
inline FunctionTranslator<RetType, FuncType (*)(FuncParams...), Params...>
NGenerateCallback(NCallback<RetType, Params...>*, FuncType (*fct)(FuncParams...))
{
    return FunctionTranslator<RetType, FuncType (*)(FuncParams...), Params...>(fct);
}

template <typename RetType, typename Callee, typename FuncType, typename CallType, typename... Params, typename... FuncParams>
inline MemberTranslator<RetType, Callee, FuncType (CallType::*)(FuncParams...), Params...>
NGenerateCallback(NCallback<RetType, Params...>*, Callee& _instance, FuncType (CallType::*const& _method)(FuncParams...))
{
    typedef FuncType (CallType::*MemFunc)(FuncParams...);
    return MemberTranslator<RetType, Callee, MemFunc, Params...>(_instance, _method);
}

template <typename RetType, typename Callee, typename FuncType, typename CallType, typename... Params, typename... FuncParams>
inline MemberTranslator<RetType, const Callee, FuncType (CallType::*)(FuncParams...) const, Params...>
NGenerateCallback(NCallback<RetType, Params...>*, const Callee& _instance, FuncType (CallType::*const& _method)(FuncParams...) const)
{
    typedef FuncType (CallType::*MemFunc)(FuncParams...) const;
    return MemberTranslator<RetType, const Callee, MemFunc, Params...>(_instance, _method);
}
