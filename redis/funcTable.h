#ifndef _FUNCTABLE_H
#define _FUNCTABLE_H

//#include<unordered_map>

#include<functional>


namespace netlib{

class dataBase;



class funcMap{
public:

	funcMap(){}


	void destroy(dataBase&);     

	bool equal(const dataBase& lhs, const dataBase& rhs);

	size_t hash(const dataBase& rhs);	

	size_t hash(const dataBase& rhs) const;


private:

};


}

#endif


