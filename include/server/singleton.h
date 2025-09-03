

#ifndef ASIOUTILS_UTILITY_SINGLETON_H
#define ASIOUTILS_UTILITY_SINGLETON_H

namespace AsioUtils {

    template <typename T>
    class Singleton
    {
        friend T;

    public:
        Singleton(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;

        Singleton& operator=(const Singleton&) = delete;
        Singleton& operator=(Singleton &&) = delete;

        static T& GetInstance()
        {
            static T instance;
            return instance;
        }

    private:
        Singleton() noexcept = default;
        ~Singleton() noexcept = default;
    };


}

#endif
