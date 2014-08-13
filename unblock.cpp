//
// 修改自 https://sites.google.com/site/liuxinyu95/softdevbooksbook1essay12chn
//

#include <iostream>
#include <map>
#include <list>
#include <deque>
#include <algorithm>
#include <cstdlib>

struct Point{
	Point():x(0), y(0){}
	Point(int _x, int _y): x(_x), y(_y){}
	Point(const Point& ref):x(ref.x), y(ref.y){}
	virtual ~Point(){}

	const Point& operator=(const Point& ref){
		x = ref.x;
		y = ref.y;
		return *this;
	};

	virtual const bool operator ==(const Point& ref) const{
		return (x == ref.x) && (y == ref.y);
	}

	const Point operator+ (const Point& ref) const{
		return Point(x+ref.x, y+ref.y);
	}

	const Point& operator+= (const Point& ref){
		x+=ref.x;
		y+=ref.y;
		return *this;
	}

	int x;
	int y;
};

struct Size: public Point{
	Size():Point(){}
	Size(int w, int h):Point(w, h){}
	const int width()  const { return Point::x; };
	const int height() const { return Point::y; };
	bool operator==(const Size& ref) const { return width()==ref.width() && height()==ref.height(); }
	bool operator!=(const Size& ref) const { return !(*this == ref); }
};

class Block{
public:
	Block(){}
	Block(const Point& p, const Size& sz, char v)
		:_topLeft(p), _size(sz), _value(v){};

	const Block& operator = (const Block& ref){
		if( *this == ref ) return *this;
		_topLeft = ref._topLeft;
		_size = ref._size;
		_value = ref._value;
		return *this;
	}

	const bool operator == (const Block& ref) const {
		return (_topLeft == ref._topLeft) && (_size == ref._size) && (_value == ref._value);
	}

	Block at(const Point& position){
		return Block(position, _size, _value);
	}

	const Size& size() const { return _size; }
	const Point& topLeft() const { return _topLeft; }
	Point& topLeft() { return _topLeft; }
	const int value() const {return _value; }

public:
	//
	// stocked block enumerations
	//
	static Block Target;
	static Block Solder1;
	static Block Solder2;
	static Block Solder3;
	static Block Solder4;
	static Block Solder5;
	static Block Solder6;
	static Block Solder7;
	static Block Solder8;
	static Block Solder9;
	static Block SolderA;
	static Block SolderB;
	static Block SolderC;

private:
	Point _topLeft;
	Size  _size;
	char _value;
};

template<class T> class Step{
public:
	Step(): _prev(0), _number(0){};
	Step(const T& value): _prev(0), _number(0), _value(value){}
	Step(Step* prev, const T& value): _prev(prev), _value(value){
		if(_prev)
			_number=_prev->_number+1;
	}

	const T& value() const   { return _value; }
	const Step* prev() const { return _prev; }
	const int number() const { return _number; }

	struct TestEqual{
		TestEqual(const T& x):ref(x){}
		bool operator() (const Step* p) const{
			// return p->value().isSameLayout(ref) || p->value().isMirrorLayout(ref);
			return p->value().isSameLayout(ref);
		}
		const T& ref;
	};

	template<class StepPtrContainer>
	static bool findStep(StepPtrContainer coll, const T& value){
		return std::find_if( coll.begin(), coll.end(),
					TestEqual(value))!=coll.end();
	}

private:
	Step* _prev;
	int   _number;
	T     _value;
};

class Klotski{
public:
	static const int COL=6;
	static const int ROW=6;

	Klotski(){
		clearMap();
	}

	Klotski(const Klotski& ref): blocks(ref.blocks){ updateMap(); }

	const Klotski& operator=(const Klotski& ref){
		if( *this == ref ) return *this;
		blocks = ref.blocks;
		updateMap();
		return *this;
	}

	const bool operator == (const Klotski& ref) const { return blocks == ref.blocks; }

	void addBlock(const Block& block){
		blocks[block.value()] = block;
		updateMap(block);
	}

	const bool isSolved() const {
		//compare Target
		std::map<char, Block>::const_iterator it = blocks.find('x');
		return Point(4, 2) == (it->second).topLeft();
	}

	bool isSameLayout(const Klotski& ref) const{
		// compare the layoutMap
		for(int y=0; y<ROW; ++y)
			for(int x=0; x<COL; ++x)
				if(layoutMap[y][x]!=ref.layoutMap[y][x])
					return false;

		return true;
	}

	bool isMirrorLayout(const Klotski& ref) const{
		for(int y=0; y<ROW; ++y)
			for(int x=0; x<COL; ++x)
				if(layoutMap[y][x]!=ref.layoutMap[y][COL-1-x])
					return false;

		return true;
	}

	template<class Container>
	std::list<Klotski> move(const Container& tried) const{
		std::list<Klotski> res;

		// left, right, up, down
		const int dx[4] = {-1, 1,  0, 0 };
		const int dy[4] = { 0, 0, -1, 1 };

		for(std::map<char, Block>::const_iterator it = blocks.begin();
			it!=blocks.end(); ++it)
		{
			for(int i=0; i<4; ++i){
				if (it->second.size().width() > it->second.size().height()) { // w > h: can't up, down
					if (i == 2 || i == 3) {
						continue;
					}
				} else { // w < h: can't left, right
					if (i == 0 || i == 1) {
						continue;
					}
				}
				Point delta(dx[i], dy[i]);
				if(canMove(it->second, delta)){
					Klotski newLayout(*this);
					newLayout.moveBlock(it->second, delta);
					if(!Step<Klotski>::findStep(tried, newLayout))
						res.push_back(newLayout);
				}
			}
		}
		return res;
	}

	void print() const{
		for(int y=0; y<ROW; ++y){
			for(int x=0; x<COL; ++x)
				std::cout<<map[y][x]<<" ";
			std::cout<<"\n";
		}
	}

	void printLayout() const{
		for(int y=0; y<ROW; ++y){
			for(int x=0; x<COL; ++x){
				const Size& sz = layoutMap[y][x];
				int value = sz.width()*10+sz.height();
				std::cout<<value<<" ";
			}
			std::cout<<"\n";
		}
	}

private:
	bool canMove(const Block& block, const Point& delta) const{
		for(int i=0; i<block.size().height(); ++i)
			for(int j=0; j<block.size().width(); ++j)
			{
				int x = block.topLeft().x + j + delta.x;
				int y = block.topLeft().y + i + delta.y;

				if( x<0 || x>=COL || y<0 || y>=ROW)
					return false;

				if(map[y][x] !='0' && map[y][x] !=block.value())
					return false;
			}

		return true;
	}

	const Klotski& moveBlock(const Block& block, const Point& delta){
		Block& ref = blocks[block.value()];
		clearMap(ref);
		ref.topLeft()+=delta;
		updateMap(ref);
		return *this;
	}

	void clearMap(){
		for(int y=0; y<ROW; ++y)
			for(int x=0; x<COL; ++x){
				map[y][x]='0';
				layoutMap[y][x]=Size(0,0);
			}
	}

	void clearMap(const Block& block){
		for(int i=0; i<block.size().height(); ++i)
			for(int j=0; j<block.size().width(); ++j){
				map[block.topLeft().y+i][block.topLeft().x+j] = '0';
				layoutMap[block.topLeft().y+i][block.topLeft().x+j] = Size(0, 0);
			}
	}

	void updateMap(){
		clearMap();
		for(std::map<char, Block>::iterator it = blocks.begin();
			it!=blocks.end(); ++it)
				updateMap(it->second);
	}

	void updateMap(const Block& block){
		for(int i=0; i<block.size().height(); ++i)
			for(int j=0; j<block.size().width(); ++j){
				map[block.topLeft().y+i][block.topLeft().x+j] = block.value();
				layoutMap[block.topLeft().y+i][block.topLeft().x+j] = block.size();
			}
	}

	char map[ROW][COL];
	Size layoutMap[ROW][COL];
	std::map<char, Block> blocks;
};

class KlotskiSolver{
public:
	typedef Step<Klotski> KlotskiStep;
	typedef std::deque<KlotskiStep*> StepQueue;
	typedef std::list<KlotskiStep*>  StepList;

	~KlotskiSolver(){
		for(StepQueue::iterator it = steps.begin();
			it!=steps.end(); ++it)
				delete (*it);

		for(StepList::iterator it = tried.begin();
			it!=tried.end(); ++it)
				delete (*it);
	}

	// must use width-first search!!
	bool run(const Klotski& layout){
		steps.push_back(new KlotskiStep(layout));

		while(!steps.empty()){
			KlotskiStep* step = steps.front();

			//
			std::cout<<"try step: ["<<step->number()<<"]\n";
			//

			if(step->value().isSolved()){
				printSteps(step);
				return true;
			}
			else{
				steps.pop_front();
				tried.push_back(step);
				std::list<Klotski> nextLayouts = step->value().move(tried);

				for(std::list<Klotski>::iterator it=nextLayouts.begin();
					it!=nextLayouts.end(); ++it)
				{
					if(!KlotskiStep::findStep(steps, *it))
						steps.push_back(new KlotskiStep(step, *it));
				}
			}
		}
		return false;
	}

private:

	int printSteps(const Step<Klotski>* step){
		if(!step->prev()){
			std::cout<<"=============step:[0]==============\n";
			step->value().print();
			return 0;
		}
		else{
			int i=1+printSteps(step->prev());
			std::cout<<"=============step:["<<i<<"]==============\n";
			step->value().print();
			return i;
		}
	};

	StepQueue steps;
	StepList  tried;
};

//
// example
//
Block Block::Target     = Block(Point(0, 2), Size(2, 1), 'x');
Block Block::Solder1    = Block(Point(0, 0), Size(3, 1), '1');
Block Block::Solder2    = Block(Point(3, 0), Size(2, 1), '2');
Block Block::Solder3    = Block(Point(5, 0), Size(1, 2), '3');
Block Block::Solder4    = Block(Point(0, 1), Size(3, 1), '4');
Block Block::Solder5    = Block(Point(3, 1), Size(1, 2), '5');
Block Block::Solder6    = Block(Point(2, 2), Size(1, 2), '6');
Block Block::Solder7    = Block(Point(5, 2), Size(1, 2), '7');
Block Block::Solder8    = Block(Point(3, 3), Size(2, 1), '8');
Block Block::Solder9    = Block(Point(0, 4), Size(1, 2), '9');
Block Block::SolderA    = Block(Point(2, 4), Size(2, 1), 'A');
Block Block::SolderB    = Block(Point(4, 4), Size(1, 2), 'B');
Block Block::SolderC    = Block(Point(1, 5), Size(3, 1), 'C');

Klotski createLayout(){
	std::cout<<"=============example===========\n";
	Klotski game;
	game.addBlock(Block::Target);
	game.addBlock(Block::Solder1);
	game.addBlock(Block::Solder2);
	game.addBlock(Block::Solder3);
	game.addBlock(Block::Solder4);
	game.addBlock(Block::Solder5);
	game.addBlock(Block::Solder6);
	game.addBlock(Block::Solder7);
	game.addBlock(Block::Solder8);
	game.addBlock(Block::Solder9);
	game.addBlock(Block::SolderA);
	game.addBlock(Block::SolderB);
	game.addBlock(Block::SolderC);
	return game;
}

int main(int argc, char* argv[]){
	KlotskiSolver solver;
	solver.run(createLayout());
}
