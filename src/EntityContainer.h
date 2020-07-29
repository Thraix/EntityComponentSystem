/* #pragma once */

/* #include <functional> */
/* #include <iostream> */

/* #include "Config.h" */

/* namespace ecs */
/* { */
/*   template <typename iterator, typename...Components> */
/*   class EntityIterator */
/*   { */
/*     public: */

/*       using difference_type = void; */
/*       using value_type = Entity; */
/*       using pointer = void; */
/*       using reference = Entity; */
/*       using iterator_category = std::bidirectional_iterator_tag; */

/*       iterator it; */
/*       iterator last; */

/*       EntityIterator(iterator it, iterator last) */
/*         : it{it}, last{last} */
/*       { */
/*         IncreaseToValid(); */
/*       } */

/*       void IncreaseToValid() */
/*       { */
/*         while(it != last && !(*it)->template HasComponents<Components...>()) */
/*         { */
/*           ++it; */
/*         } */
/*       } */

/*       value_type operator*() */
/*       { */
/*         return *it; */
/*       } */

/*       EntityIterator<iterator, Components...>& operator++() */
/*       { */
/*         if(it != last) */
/*           ++it; */
/*         IncreaseToValid(); */
/*         return *this; */
/*       } */

/*       EntityIterator<iterator, Components...> operator++(int) */
/*       { */
/*         auto tmp{*this}; */
/*         ++*this; */
/*         return tmp; */
/*       } */

/*       bool operator==(EntityIterator<iterator, Components...> other) */
/*       { */
/*         return it == other.it; */
/*       } */

/*       bool operator!=(EntityIterator<iterator, Components...> other) */
/*       { */
/*         return it != other.it; */
/*       } */
/*   }; */

/*   template <typename Container, typename... Components> */
/*   class EntityContainer */
/*   { */
/*     using iterator = decltype(std::begin(std::declval<Container>())); */
/*     private: */
/*       EntityIterator<iterator,Components...> first; */
/*       EntityIterator<iterator,Components...> last; */

/*     public: */
/*       EntityContainer(Container& c) */
/*         : first{std::begin(c), std::end(c)}, last{std::end(c), std::end(c)} */
/*       {} */

/*       EntityIterator<iterator,Components...> begin() { return first;} */
/*       EntityIterator<iterator,Components...> end() { return last;} */

/*   }; */
/* } */
