#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

//禁止复制
class NonCopyable
{
public:
private:
protected:
    NonCopyable() {}
    ~NonCopyable() {}

    NonCopyable(const NonCopyable &rhs) = delete;
    NonCopyable &operator=(const NonCopyable &rhs) = delete;

private:

};


#endif
