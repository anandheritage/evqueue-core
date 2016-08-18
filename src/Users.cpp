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

#include <Users.h>
#include <User.h>
#include <Exception.h>
#include <DB.h>
#include <Logger.h>
#include <SocketQuerySAX2Handler.h>
#include <QueryResponse.h>

#include <xqilla/xqilla-dom3.hpp>

Users *Users::instance = 0;

using namespace std;
using namespace xercesc;

Users::Users():APIObjectList()
{
	instance = this;
	
	Reload();
}

Users::~Users()
{
}

void Users::Reload(void)
{
	Logger::Log(LOG_NOTICE,"[ Users ] Reloading users definitions");
	
	pthread_mutex_lock(&lock);
	
	clear();
	
	// Update
	DB db;
	DB db2(&db);
	db.Query("SELECT user_login FROM t_user");
	
	while(db.FetchRow())
		add(0,db.GetField(0),new User(&db2,db.GetField(0)));
	
	pthread_mutex_unlock(&lock);
}

bool Users::HandleQuery(SocketQuerySAX2Handler *saxh, QueryResponse *response)
{
	Users *users = Users::GetInstance();
	
	string action = saxh->GetRootAttribute("action");
	
	if(action=="list")
	{
		pthread_mutex_lock(&users->lock);
		
		for(auto it = users->objects_name.begin(); it!=users->objects_name.end(); it++)
		{
			User user = *it->second;
			DOMElement *node = (DOMElement *)response->AppendXML("<user />");
			node->setAttribute(X("name"),X(user.GetName().c_str()));
			node->setAttribute(X("profile"),X(user.GetProfile().c_str()));
		}
		
		pthread_mutex_unlock(&users->lock);
		
		return true;
	}
	
	return false;
}