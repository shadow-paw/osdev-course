#ifndef KERNEL_KVECTOR_H_
#define KERNEL_KVECTOR_H_

#include "kstring.h"
#include "kdebug.h"

namespace kernel {

template <class T>
class kvector {
  public:
    kvector(): _data(nullptr), _count(0), _allocated(0) {}
    ~kvector() {
        if (_data) {
            for (unsigned int i=0; i<_count; i++) {
                delete _data[i];
            }
            delete(_data);
            _data = nullptr;
        }
    }
    unsigned int count() const { return _count; }
    const T& operator [](unsigned int index) const {
        // TODO: out_of_range exception
        return *_data[index];
    }
    T& operator [](unsigned int index){
        // TODO: out_of_range exception
        return *_data[index];
    }
    T& append(const T& o) {
        if (_count +1 >= _allocated) {
            T** _new = (T**)kmalloc(_count + 4);
            if (_count > 0) {
                kmemcpy(_new, _data, _count * sizeof(T*));
            }
            if (_data) {
                kfree(_data);
            }
            _data = _new;
            _allocated = _count + 4;
        }
        T* moved = new T(o);
        _data[_count] = moved;
        _count ++;
        return *moved;
    }
  private:
    T** _data;
    unsigned int _count, _allocated;
};

}  // namespace kernel

#endif  // KERNEL_KVECTOR_H_
