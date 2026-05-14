// #include <iostream>
//
// template <typename T> class myvector {
// private:
//   unsigned int size;
//   unsigned int allocated;
//   T *tab;
//
// public:
//   myvector(int s = 20) : size(s) {
//     allocated = 0;
//     tab = new T[s];
//   }
//
//   void push_back(T s) {
//     if (allocated >= size) {
//       T *temp = new T[size * 2];
//       for (int i = 0; i < size; ++i) {
//         temp[i] = tab[i];
//       }
//       size *= 2;
//       delete[] tab;
//       tab = temp;
//     }
//
//     tab[allocated] = s;
//     ++allocated;
//   }
//
//   void print_all() {
//     for (int i = 0; i < allocated; ++i) {
//       std::cout << tab[i] << ' ';
//     }
//     std::cout << '\n';
//   }
//
//   ~myvector() { delete[] tab; }
// };
//
// int main() {
//   myvector<int> test(20);
//   for (int i = 0; i < 30; ++i) {
//     test.push_back(i);
//   }
//
//   test.print_all();
//
//   return 0;
// };
//

// #include <forward_list>
// #include <iostream>
//
// int main() {
//   std::forward_list<int> ll_replacement;
//   auto it = ll_replacement.before_begin();
//   it = ll_replacement.insert_after(it, 0);
//   it = ll_replacement.insert_after(it, 1);
//   it = ll_replacement.insert_after(it, 2);
//   it = ll_replacement.insert_after(it, 3);
//   it = ll_replacement.insert_after(it, 4);
//
//   for (auto i : ll_replacement) {
//     std::cout << i << ' ';
//   }
//   std::cout << '\n';
// }

#include <chrono>
#include <iostream>
#include <memory>

// ---------- Virtual Inheritance ----------
struct VirtualBase {
  virtual void process() {
    volatile int sum = 0;
    for (int i = 0; i < 1000000; ++i)
      sum += i;
  }

  virtual ~VirtualBase() = default;
};

struct VirtualDerived : public VirtualBase {
  void process() override {
    volatile int sum = 0;
    for (int i = 0; i < 1000000; ++i)
      sum += i;
  }
};

// ---------- CRTP ----------
template <typename Derived> struct CRTPBase {
  void process() { static_cast<Derived *>(this)->process_impl(); }
};

struct CRTPDerived : public CRTPBase<CRTPDerived> {
  void process_impl() {
    volatile int sum = 0;
    for (int i = 0; i < 1000000; ++i)
      sum += i;
  }
};

// ---------- Timing Helpers ----------
template <typename Func> double benchmark(const std::string &label, Func &&f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::micro> duration = end - start;
  return duration.count();
}

int main() {
  double v_duration = 0.0, c_duration = 0.0;
  for (int i = 0; i < 1000; ++i) {
    VirtualBase *vobj = new VirtualDerived();
    v_duration += benchmark("Virtual dispatch", [&]() { vobj->process(); });
    delete vobj;

    CRTPDerived cobj;
    c_duration +=
        benchmark("CRTP (static) dispatch", [&]() { cobj.process(); });
  }

  std::cout << "Virtual dispatch average time: " << v_duration / 1000 << '\n';
  std::cout << "CRTP dispatch average time: " << c_duration / 1000 << '\n';

  return 0;
}
