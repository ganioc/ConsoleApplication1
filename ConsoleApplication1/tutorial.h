#pragma once
#include <iostream>

namespace tutorial {
	struct G
	{
		void operator()(int i);
	};

	void doSomeWork();

	template <typename ... F> 
	struct overload_set;

	template <typename F1>
	struct overload_set<F1> : public F1 {
		overload_set(F1&& f1): F1(std::move(f1)){}
		overload_set(const F1& f1) : F1(f1) {}
		using F1::operator();
	};

	template <typename F1,typename ... F>
	struct overload_set<F1, F ...> : public F1, public overload_set<F ...> {
		overload_set(F1&& f1, F&& ... f) : F1(std::move(f1)), overload_set<F ...>(std::forward<F>(f)...) {}
		overload_set(const F1& f1, F&& ...f):F1(f1),overload_set<F ...>(std::forward<F>(f)...) {}
		using overload_set<F ...>::operator();
		using F1::operator();

	};

	template <typename ... F>
	auto overload(F&& ... f) {
		return overload_set<F ...>(std::forward<F>(f) ...);
	}

	void testTemplate();

	// portotype definition
	class BaseP {
	public:
		virtual BaseP* clone()=0;
		virtual void doSomething() = 0;
	};
	class FactoryP
	{
	public:
		FactoryP();
		~FactoryP();
		static BaseP* makeBase(int choice);

	private:
		static BaseP* s_prototypes[3];
	};

	class A:public BaseP
	{
	public:
		A();
		~A();
		BaseP* clone() {
			return new A;
		}
		void doSomething() {
			std::cout << "A is now doing something\n";
		}
	private:

	};

	class B:public BaseP
	{
	public:
		B();
		~B();
		BaseP* clone() {
			return new B;
		}
		void doSomething() {
			std::cout << "B is now doing something\n";
		}
	private:

	};
	void testPrototype();

	// singleton
	class Single {
	public:
		static Single* getInstance(void) {
			if (!instance_) {
				std::cout << "1st getInstance\n";
				instance_ = new Single();
			}
			return instance_;
		}
	private:
		Single() {
			std::cout << "Single\n";
		}
		static Single* instance_;
	};
	void testSingleton();

}