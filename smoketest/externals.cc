#include <bayeux/datatools/properties.h>

int main(int, char**)
{
  datatools::properties x{};
  x.store("hello", std::vector<int>{1,2,3,4});

  return 0;
}
