#ifndef ASIOUTILS_THREADS_LOCKER_H
#define ASIOUTILS_THREADS_LOCKER_H

namespace AsioUtils {

    template <class T>
    class Locker
    {
    public:
        explicit Locker(T& primitive) : _primitive(primitive) { _primitive.Lock(); }
        Locker(const Locker&) = delete;
        Locker(Locker&&) = delete;
        ~Locker() { _primitive.Unlock(); }

        Locker& operator=(const Locker&) = delete;
        Locker& operator=(Locker&&) = delete;

    private:
        T& _primitive;
    };

    template <class T>
    class ReadLocker
    {
    public:
        explicit ReadLocker(T& primitive) : _primitive(primitive) { _primitive.LockRead(); }
        ReadLocker(const ReadLocker&) = delete;
        ReadLocker(ReadLocker&&) = delete;
        ~ReadLocker() { _primitive.UnlockRead(); }

        ReadLocker& operator=(const ReadLocker&) = delete;
        ReadLocker& operator=(ReadLocker&&) = delete;

    private:
        T& _primitive;
    };

    template <class T>
    class WriteLocker
    {
    public:
        explicit WriteLocker(T& primitive) : _primitive(primitive) { _primitive.LockWrite(); }
        WriteLocker(const WriteLocker&) = delete;
        WriteLocker(WriteLocker&&) = delete;
        ~WriteLocker() { _primitive.UnlockWrite(); }

        WriteLocker& operator=(const WriteLocker&) = delete;
        WriteLocker& operator=(WriteLocker&&) = delete;

    private:
        T& _primitive;
    };

}

#endif