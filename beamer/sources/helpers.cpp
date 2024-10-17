#include "raylib.h"
#include "swept-aabb.h"
#include "operators.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm> // For std::max_element

class Room {
    public:
    Room(std::vector<std::wstring> room)
    {
        for(int i = 0; i<room.size(); i++)
        {
            _room.push_back(room[i]);
        }
    }

    wchar_t GetTile(int x, int y)
    {
        if ((y>=0 && y < GetHeight()) && (x >= 0 && x < _room.at(y).length()))
            return _room[y][x];
        else 
            return L' ';
            
    }

    void SetTile(int x, int y, wchar_t c)
    {
        if ((x>=0 && x < GetHeight()) && (y >= 0 && y < _room.at(x).length()))
            _room[y][x] = c;
    }

    int GetMaxWidth()
    {
        return std::max_element(_room.begin(), _room.end(), 
                                      [](const std::wstring& a, const std::wstring& b) {
                                          return a.size() < b.size();
                                      })->size();
    }

    int GetHeight()
    {
        return _room.size();
    }
    private:
    std::vector<std::wstring> _room;
         
};