#include <iostream>

class Order {
  int price = 0;
  int quantity = 0;

public:
  void print_data() { std::cout << price << ' ' << quantity << std::endl; }
};

int main() {
  Order *o = new Order();
  int *price_ptr = (int *)o;
  *price_ptr = 3;

  o->print_data();

  return 0;
}
