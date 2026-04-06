#include <iostream>

class Trade {
private:
  float price;
  float quantity;

public:
  Trade(float other_price, float other_quantity)
      : price(other_price), quantity(other_quantity) {}

  void setPrice(float price) { this->price = price; }

  void setQuantity(float quantity) { this->quantity = quantity; }

  void printTrade() {
    std::cout << "Trade value: " << price * quantity << std::endl;
  }
};

// int main() {
//   Trade t(10, 10);
//   t.printTrade();
// }

#include <cstdint>

class Position {
public:
  double x;
  double y;
  double z;
  char a;
};

// int main() {
//   Position pos;
//   pos.x = 1.0;
//   pos.y = 2.0;
//   pos.z = 3.0;
//   double* raw = (double*)&pos;
//   std::cout << "pos.x: " << pos.x << " raw[0]: " << raw[0] << '\n';
//   std::cout << "pos.y: " << pos.y << " raw[1]: " << raw[1] << '\n';
//   std::cout << "pos.z: " << pos.z << " raw[2]: " << raw[2] << '\n';
//
//   std::cout << "size of pos: " << sizeof(pos) << '\n';
// }

#include <chrono>

void allocate_2d_matrix(int ***matrix, int rows, int cols) {
  int **m = new int *[rows];
  for (int i = 0; i < rows; ++i)
    m[i] = new int[cols];

  matrix = &m;
}

void multiply_matrices(int **A, int **B, int ***C, int rows, int inner,
                       int cols) {
  allocate_2d_matrix(C, rows, cols);
  int curr;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      curr = 0;
      for (int k = 0; k < inner; ++k) {
        curr += A[i][k] * B[k][j];
      }
      *C[i][j] = curr;
    }
  }
}

void free_2d_matrix(int **matrix, int rows) {
  for (int i = 0; i < rows; ++i)
    delete[] matrix[i];
}

// int main() {
//   int **A;
//   int **B;
//   int **C;
//
//   int rows = 2, inner = 3, cols = 2;
//
//   std::chrono::high_resolution_clock::time_point t1 =
//   std::chrono::high_resolution_clock::now(); allocate_2d_matrix(&A, rows,
//   inner); allocate_2d_matrix(&B, inner, cols); multiply_matrices(A, B, &C,
//   rows, inner, cols); std::chrono::high_resolution_clock::time_point t2 =
//   std::chrono::high_resolution_clock::now();
//
//   free_2d_matrix(A, rows);
//   free_2d_matrix(B, inner);
//   free_2d_matrix(C, cols);
//
//   std::cout << "Time to multiply matrices: " <<
//   std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() <<
//   '\n';
// }

void int_swap(int &reference, int *pointer) {
  reference = 2;
  *pointer = 2;
}

int main() {
  int a = 1, b = 1;
  int_swap(a, &b);
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
}
