#pragma once

/**
 * 单例模板
 * @tparam T 单例实例
 */
template <typename T>
class Singleton
{
public:
	Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
	~Singleton() = default;

	static T& Instance(); // 获取单例实例
};


// 单例的显式特化：必须恰在一个编译单元里定义（OV_DEFINE_*），
// 其它要调用 Instance() 的编译单元必须先声明（OV_DECLARE_*）。
// 故意不提供泛型 Instance() 定义 —— 忘记注册的编译单元会在链接期报错，
// 而不是悄悄生成一份只属于自己的函数内静态对象。
//
// 这两个宏只在 DLL 内部使用：单例不导出，UE 侧看不到这个头。
#define OV_DECLARE_DLL_SINGLETON(Type) \
	template <> Type& Singleton<Type>::Instance();

#define OV_DEFINE_DLL_SINGLETON(Type) \
	template <> Type& Singleton<Type>::Instance() \
	{ \
		static Type instance_; \
		return instance_; \
	}
