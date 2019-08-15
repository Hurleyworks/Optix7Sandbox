/** ==========================================================================
* 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
*
* Example of a normal std::queue protected by a mutex for operations,
* making it safe for thread communication, using std::mutex from C++0x with
* the help from the std::thread library from JustSoftwareSolutions
* ref: http://www.stdthread.co.uk/doc/headers/mutex.html
*
* This exampel  was totally inspired by Anthony Williams lock-based data structures in
* Ref: "C++ Concurrency In Action" http://www.manning.com/williams */

#pragma once

/** Multiple producer, multiple consumer thread safe queue
* Since 'return by reference' is used this queue won't throw */
template<typename T>
class SharedQueue {
	std::queue<T> queue_;
	std::queue<T> emptyQueue_;
	mutable std::mutex m_;
	std::condition_variable data_cond_;

	SharedQueue& operator=(const SharedQueue&) = delete;
	SharedQueue(const SharedQueue& other) = delete;

public:

	SharedQueue() {}

	void push(T item) {
		{
			std::lock_guard<std::mutex> lock(m_);
			queue_.push(std::move(item));
		}
		data_cond_.notify_one();
	}

	/// \return immediately, with true if successful retrieval
	bool try_and_pop(T& popped_item) {
		std::lock_guard<std::mutex> lock(m_);
		if (queue_.empty()) {
			return false;
		}
		popped_item = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	/// Try to retrieve, if no items, wait till an item is available and try again
	void wait_and_pop(T& popped_item) {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty()) {
			data_cond_.wait(lock);
			//  This 'while' loop is equal to
			//  data_cond_.wait(lock, [](bool result){return !queue_.empty();});
		}
		popped_item = std::move(queue_.front());
		queue_.pop();
	}

	bool empty() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.empty();
	}

	unsigned size() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.size();
	}
	void clear()
	{
		// Huh? no clear function??????
		// so assign an empty queue
		queue_ = emptyQueue_;
	}
};


// From Anthony Williams C++ Concurrency

struct empty_stack: std::exception
{
    const char* what() const throw()
    {
        return "empty stack";
    }
};

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack(){}
    threadsafe_stack(const threadsafe_stack& other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data=other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }
    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        std::shared_ptr<T> const res(
            std::make_shared<T>(std::move(data.top())));
        data.pop();
        return res;
    }
    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        value=std::move(data.top());
        data.pop();
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

// From Anthony Williams C++ Concurrency

template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue()
    {}
    threadsafe_queue(threadsafe_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue=other.data_queue;
    }

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        value=data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty)
            return false;
        value=data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }

	size_t size() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.size();
	}
};
