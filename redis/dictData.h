#ifndef _DICT_DATA_H
#define _DICT_DATA_H


namespace netlib{

class StringPiece;

class Data{
public:
        Data(const StringPiece&, const StringPiece&);
        StringPiece key;
        StringPiece val;
};


}
#endif
