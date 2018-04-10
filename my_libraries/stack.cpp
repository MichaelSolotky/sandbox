template <typename T>
class stack {
public:
	T *arr;
	int top;
	size_t total_size;
	stack() {
		top = -1;
		total_size = 536870912; // 2 ^ 29
		arr = (T *) malloc(total_size);
	}
	stack(size_t size) {
		top = -1;
		total_size = size * sizeof(T)
		arr = (T *) malloc(total_size);
	}
	~stack() { free(arr); }
	void push(T elem) { arr[++top] = elem; }
	void pop(T &dst) { T = arr[top--]; }
};

