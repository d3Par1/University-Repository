#include <iostream>
#include <typeinfo>
#include <string>
#include <cxxabi.h>
using namespace std;

string demangle(const char* name) {
#ifdef __GNUG__ 
    int status = -1;
    char* demangledName = abi::__cxa_demangle(name, NULL, NULL, &status);
    if (status == 0) {
        string result(demangledName);
        free(demangledName);
        return result;
    }
    return name;
#else
    return name;
#endif
}

class Base {
public:
    virtual void virtualFunc() {
        cout << "Base::virtualFunc()" << endl;
    }
    virtual ~Base() {} 
};

class Derived : public Base {
public:
    void virtualFunc() override {
        cout << "Derived::virtualFunc()" << endl;
    }
};

void demo_fundamental_types() {
    cout << "\n=== Fundamental Types demonstration ===\n";
    
    int i = 10;
    double d = 3.14;
    char c = 'A';
    
    cout << "Type of i: " << typeid(i).name() << " (demangled: " << demangle(typeid(i).name()) << ")" << endl;
    cout << "Type of d: " << typeid(d).name() << " (demangled: " << demangle(typeid(d).name()) << ")" << endl;
    cout << "Type of c: " << typeid(c).name() << " (demangled: " << demangle(typeid(c).name()) << ")" << endl;
    
    cout << "\nComparing types:\n";
    cout << "typeid(i) == typeid(int): " << (typeid(i) == typeid(int) ? "true" : "false") << endl;
    cout << "typeid(d) == typeid(int): " << (typeid(d) == typeid(int) ? "true" : "false") << endl;
}

void demo_polymorphic_classes() {
    cout << "\n=== Polymorphic Classes demonstration ===\n";
    
    Base baseObj;
    Derived derivedObj;
    
    cout << "Type of baseObj: " << typeid(baseObj).name() << " (demangled: " << demangle(typeid(baseObj).name()) << ")" << endl;
    cout << "Type of derivedObj: " << typeid(derivedObj).name() << " (demangled: " << demangle(typeid(derivedObj).name()) << ")" << endl;
    
    Base* pBase1 = &baseObj;
    Base* pBase2 = &derivedObj;
    
    cout << "\nTypes through pointers:\n";
    cout << "Type of *pBase1: " << typeid(*pBase1).name() << " (demangled: " << demangle(typeid(*pBase1).name()) << ")" << endl;
    cout << "Type of *pBase2: " << typeid(*pBase2).name() << " (demangled: " << demangle(typeid(*pBase2).name()) << ")" << endl;
    
    cout << "\nPolymorphic behavior:\n";
    pBase1->virtualFunc();
    pBase2->virtualFunc();
}

void demo_dynamic_cast() {
    cout << "\n=== dynamic_cast demonstration ===\n";
    
    Base* pBase = new Derived();
    
    cout << "Type of *pBase: " << typeid(*pBase).name() << " (demangled: " << demangle(typeid(*pBase).name()) << ")" << endl;
    
    Derived* pDerived = dynamic_cast<Derived*>(pBase);
    
    if (pDerived) {
        cout << "dynamic_cast successful: pBase is pointing to a Derived object" << endl;
    } else {
        cout << "dynamic_cast failed: pBase is not pointing to a Derived object" << endl;
    }
    
    Base* pBase2 = new Base();
    Derived* pDerived2 = dynamic_cast<Derived*>(pBase2);
    
    if (pDerived2) {
        cout << "dynamic_cast successful: pBase2 is pointing to a Derived object" << endl;
    } else {
        cout << "dynamic_cast failed: pBase2 is not pointing to a Derived object" << endl;
    }
    
    delete pBase;
    delete pBase2;
}

void demo_type_info_methods() {
    cout << "\n=== std::type_info methods demonstration ===\n";
    
    const type_info& int_info = typeid(int);
    const type_info& double_info = typeid(double);
    const type_info& base_info = typeid(Base);
    const type_info& derived_info = typeid(Derived);
    
    cout << "int_info.name(): " << int_info.name() << " (demangled: " << demangle(int_info.name()) << ")" << endl;
    cout << "double_info.name(): " << double_info.name() << " (demangled: " << demangle(double_info.name()) << ")" << endl;
    
    cout << "\nHash codes:\n";
    cout << "int_info.hash_code(): " << int_info.hash_code() << endl;
    cout << "double_info.hash_code(): " << double_info.hash_code() << endl;
    cout << "base_info.hash_code(): " << base_info.hash_code() << endl;
    cout << "derived_info.hash_code(): " << derived_info.hash_code() << endl;
    
    cout << "\nOperator == and !=:\n";
    cout << "int_info == double_info: " << (int_info == double_info ? "true" : "false") << endl;
    cout << "base_info != derived_info: " << (base_info != derived_info ? "true" : "false") << endl;
    
    cout << "\nbefore() method (collation order):\n";
    cout << "int_info.before(double_info): " << (int_info.before(double_info) ? "true" : "false") << endl;
    cout << "double_info.before(int_info): " << (double_info.before(int_info) ? "true" : "false") << endl;
    cout << "base_info.before(derived_info): " << (base_info.before(derived_info) ? "true" : "false") << endl;
    cout << "derived_info.before(base_info): " << (derived_info.before(base_info) ? "true" : "false") << endl;
}

void demo_common_mistakes() {
    cout << "\n=== Common Mistakes with typeid ===\n";
    
    Base* pBase = new Derived();
    
    cout << "typeid(pBase).name(): " << typeid(pBase).name() 
         << " - це тип вказівника, а не тип об'єкта!" << endl;
    
    cout << "typeid(*pBase).name(): " << typeid(*pBase).name() 
         << " - правильно: тип об'єкта" << endl;
    
    class NonPolymorphic {};
    class DerivedNonPolymorphic : public NonPolymorphic {};
    
    NonPolymorphic* pNon = new DerivedNonPolymorphic();
    
    cout << "\nNon-polymorphic class issue:\n";
    cout << "typeid(*pNon).name(): " << typeid(*pNon).name() 
         << " - неправильно визначає тип, бо базовий клас не є поліморфним!" << endl;
    
    cout << "\nDon't forget to #include <typeinfo> before using typeid or dynamic_cast!" << endl;
    
    delete pBase;
    delete pNon;
}

int main() {
    cout << "=== <typeinfo> Header and RTTI Demonstration ===\n";
    
    demo_fundamental_types();
    demo_polymorphic_classes();
    demo_dynamic_cast();
    demo_type_info_methods();
    demo_common_mistakes();
    
    cout << "\n=== RTTI Usage Guidelines ===\n";
    cout << "1. Always include <typeinfo> header before using typeid or dynamic_cast\n";
    cout << "2. RTTI works properly only with polymorphic classes (having virtual functions)\n";
    cout << "3. Use typeid on objects (*ptr), not on pointers (ptr)\n";
    cout << "4. typeid uses == operator for comparison (not ==)\n";
    cout << "5. dynamic_cast returns nullptr for pointers if casting fails\n";
    cout << "6. dynamic_cast throws std::bad_cast for references if casting fails\n";
    cout << "7. Consider alternatives like virtual functions before using RTTI\n";
    
    return 0;
}