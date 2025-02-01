/* 
Types of Inheritance in C++
1. Single Inheritance – One class inherits from another.
2. Multiple Inheritance – A class inherits from multiple base classes.
3. Multilevel Inheritance – A class inherits from a derived class.
4. Hierarchical Inheritance – Multiple derived classes from a single base class.
5. Hybrid (Virtual) Inheritance – Combination of multiple and multilevel inheritance (resolves diamond problem).
*/

/*
| Base Class Member | public Inheritance  | protected Inheritance | private Inheritance  |
|------------------|--------------------|----------------------|---------------------|
| public          | Becomes public      | Becomes protected    | Becomes private    |
| protected       | Becomes protected   | Becomes protected    | Becomes private    |
| private         | ❌ Not inherited    | ❌ Not inherited     | ❌ Not inherited   |
Inherited Function Access Rules:
+) Public functions in the base class:

- With public inheritance: They remain public in the derived class.
- With protected inheritance: They become protected in the derived class.
- With private inheritance: They become private in the derived class.

+) Protected functions in the base class:

- With public inheritance: They become protected in the derived class.
- With protected inheritance: They remain protected in the derived class.
- With private inheritance: They become private in the derived class.

+) Private functions in the base class:

- Are not inherited by the derived class, regardless of the inheritance access specifier. These functions are private to the base class.
*/