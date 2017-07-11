#include <setjmp.h>
#include <signal.h>
#include <iostream>

jmp_buf here;

struct TestRAII {
	TestRAII(int name): name(name) {
		std::cout << __PRETTY_FUNCTION__ << " " << name << std::endl;
	}

	virtual ~TestRAII() {
		std::cout << __PRETTY_FUNCTION__ << " " << name << std::endl;
	}

	int name;
};

void hardFail() {
	TestRAII testRAII(2);
	*(static_cast<int*>(0)) = 0;
}

void sighandler(int signal) {
	longjmp(here, signal);
}

int main() {
	try {
		TestRAII testRAII(1);

		signal(SIGSEGV, sighandler);
		if (int err = setjmp(here)) {
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "signal handler %d", err);
			throw std::runtime_error(buffer);
		}

		std::cout << "Hello, World!" << std::endl;

		hardFail();
	} catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}