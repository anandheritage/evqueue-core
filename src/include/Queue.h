/*
 * This file is part of evQueue
 * 
 * evQueue is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * evQueue is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with evQueue. If not, see <http://www.gnu.org/licenses/>.
 * 
 * Author: Thibault Kummer <bob@coldsource.net>
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <xercesc/dom/DOM.hpp>
using namespace xercesc;

#include <global.h>

#include <string>
#include <deque>
#include <queue>
#include <map>

#define QUEUE_SCHEDULER_FIFO 1
#define QUEUE_SCHEDULER_PRIO 2

class WorkflowInstance;
class SocketQuerySAX2Handler;
class QueryResponse;
class User;

class Queue
{
	private:
		struct Task
		{
			WorkflowInstance *workflow_instance;
			DOMNode *task;
		};
		
		unsigned int id;
		std::string name;
		
		unsigned int concurrency;
		unsigned int size;
		unsigned int running_tasks;
		
		std::multimap<unsigned int,Task *> prio_queue;
		std::deque<Task *> queue;
		
		std::queue<Task *>cancelled_tasks;
		
		std::string wanted_scheduler;
		int scheduler;
		
		bool removed;
		
	public:
		Queue(unsigned int id,const std::string &name, int concurrency, int scheduler, const std::string &wanted_scheduler);
		~Queue();
		
		inline unsigned int GetID() { return id; }
		
		static bool CheckQueueName(const std::string &queue_name);
		inline const std::string &GetName(void) { return name; }
		
		void EnqueueTask(WorkflowInstance *workflow_instance,DOMNode *task);
		bool DequeueTask(WorkflowInstance **p_workflow_instance,DOMNode **p_task);
		bool ExecuteTask(void);
		bool TerminateTask(void);
		bool CancelTasks(unsigned int workflow_instance_id);
		
		inline unsigned int GetSize(void) { return size; }
		inline unsigned int GetRunningTasks(void) { return running_tasks; }
		
		void SetConcurrency(unsigned int concurrency);
		inline unsigned int GetConcurrency(void) { return concurrency; }
		
		void SetScheduler(unsigned int new_scheduler);
		inline unsigned int GetScheduler(void) { return scheduler; }
		const std::string &GetWantedScheduler(void) { return wanted_scheduler; }
		
		inline void Remove() { removed = true; }
		inline bool IsRemoved() { return removed; }
		
		bool IsLocked(void);
		
		static void Get(unsigned int id, QueryResponse *response);
		static void Create(const std::string &name, int concurrency, const std::string &scheduler);
		static void Edit(unsigned int id,const std::string &name, int concurrency, const std::string &scheduler);
		static void Delete(unsigned int id);
		
		static bool HandleQuery(const User &user, SocketQuerySAX2Handler *saxh, QueryResponse *response);
	
	private:
		void enqueue_task(WorkflowInstance *workflow_instance,DOMNode *task);
		void dequeue_task(WorkflowInstance **p_workflow_instance,DOMNode **p_task);
		
		static void create_edit_check(const std::string &name, int concurrency, const std::string &scheduler);
};

#endif
