#pragma once
#include <iostream>
#include <functional>

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

	void testFunction();

	// type erasure using inheritance
	template<typename T>
	class smartptr
	{
	public:
		template<typename Deleter>
		smartptr(T* p, Deleter d) : p_(p), d_(new destroy<Deleter>(d)) {}
		~smartptr() {
			(*d_)(p_);
			delete d_;
		}
		T* operator->() {
			return p_;
		}

	private:
		struct destroy_base {
			virtual void operator()(void*) = 0;
			virtual ~destroy_base() {}
		};
		template<typename Deleter>
		struct destroy : public destroy_base {
			destroy(Deleter d) : d_(d) {}
			void operator()(void* p) override {
				d_(static_cast<T*>(p));
			}
			Deleter d_;
		};

		T* p_;
		destroy_base* d_;
	};

	template<typename T>
	class smartptr2 {
	public:
		template<typename Deleter> 
		smartptr2(T* p, Deleter d) : p_(p) {
			static_assert(sizeof(Deleter) <= sizeof(buf_));
			::new(static_cast<void*>(buf_)) destroy<Deleter>(d) ;
		}
		~smartptr2() {
			destroy* d = (destroy*)buf_;
			(*d)(p_);
			d->~destroy_base();
		}
	private:
		struct destroy_base {
			virtual void operator()(void*) = 0;
			virtual ~destroy_base() {}
		};
		template<typename Deleter>
		struct destroy : public destroy_base {
			destroy(Deleter d) : d_(d) {}
			void operator()(void* p) override {
				d_(static_cast<T*>(p));
			}
			Deleter d_;
		};

		T* p_;
		alignas(8) char buf_[16];

	};

	template<typename T>
	class smartptr_te_static {
		T* p_;
		using destroy_t = void(*)(T*, void*);
		destroy_t destroy_;
		alignas(8) char buf_[8];

		template<typename Deleter>
		static void invoke_destroy(T* p, void* d) {
			(*static_cast<Deleter*> (d))(p);
		}
	public:
		template<typename Deleter>
		smartptr_te_static(T* p, Deleter d) :p_(p), destroy_(invoke_destroy<Deleter>) {
			static_assert(sizeof(Deleter) <= sizeof(buf_));
			::new(static_cast<void*>(buf_)) Deleter(d);
		}
		~smartptr_te_static() {
			this->destroy_(p_, buf_);
		}
		T* operator-> () {
			return p_; 
		}
		const T* operator->() const {
			return p_;
		}
	};

	template<typename T>
	class smartptr_te_vtable {
		T* p_;
		struct vtable_t {
			using destroy_t = void(*)(T*, void*);
			using destructor_t = void(*)(void*);
			destroy_t destroy_;
			destructor_t destructor_;
		};
		const vtable_t* vtable_ = nullptr;

		template<typename Deleter>
		constexpr static vtable_t vtable = {
			smartptr_te_vtable::template destroy<Deleter>,
			smartptr_te_vtable::template destructor<Deleter>
		};

		template<typename Deleter>
		static void destroy(T* p, void* d) {
			(*static_cast<Deleter*>(d))(p);
		}

		template<typename Deleter>
		static void destructor(void* d) {
			static_cast<Deleter*>(d)->~Deleter();
		}

		alignas(8) char buf_[8];
	public:
		template<typename Deleter>
		smartptr_te_vtable(T* p, Deleter d) :p_(p), vtable_(&vtable<Deleter>) {
			static_assert(sizeof(Deleter) <= sizeof(buf_));
			::new(static_cast<void*>(buf_)) Deleter(d);
		}
		~smartptr_te_vtable() {
			this->vtable_->destroy_(p_, buf_);
			this->vtable_->destructor_(buf_);
		}
		T* operator->() {
			return p_;
		}
		const T* operator->() const {
			return p_;
		}
	};
}