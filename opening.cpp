#include <bits/stdc++.h>
#include <fstream>

using namespace std;

// mt19937 rng(random_device{}());
mt19937 rng;
enum Cell{None, Me, You};
enum State{Continue, End, Invalid};
enum Color{Draw, Black, White};

const int SIZE = 4;
#define IDX(x, y, z) ((x) + (y) * SIZE + (z) * SIZE * SIZE)
#define BIT(x, y, z) (1uLL << IDX(x, y, z))
#define X(idx) ((idx) % SIZE)
#define Y(idx) ((idx) / SIZE % SIZE)
#define Z(idx) ((idx) / SIZE / SIZE)
const int BOARD_SIZE = SIZE * SIZE * SIZE;
const int INF = 1e9;
const int LINES_NUM = 76;

array<unsigned long long, LINES_NUM> LINES;
static const unsigned long long move_order[] = {
	//BIT(0, 0, 0) | BIT(0, 3, 0) | BIT(3, 0, 0) | BIT(3, 3, 0) |
	//BIT(1, 1, 1) | BIT(1, 2, 1) | BIT(2, 1, 1) | BIT(2, 2, 1) |
	//BIT(1, 1, 2) | BIT(1, 2, 2) | BIT(2, 1, 2) | BIT(2, 2, 2) |
	//BIT(0, 0, 3) | BIT(0, 3, 3) | BIT(3, 0, 3) | BIT(3, 3, 3),// first
	//0x9009000006609009uLL,
	0x9009066006609009uLL,//first 核
	//0x0000ffffffff0000uLL,
	0x6ff6f99ff99f0000uLL,//second 1段目以外
	//0x0000ffffffffffffuLL,//4段目以外
	//0xffff0000ffffffffuLL,//3段目以外
	//0xffffffff0000ffffuLL,//2段目以外
	//0xf99ff99ff99ff99fuLL,//真ん中以外
	//0xf99ff99ff99f0000uLL,
	//0xffffffffffff0660uLL,
	//0x0000000090090000uLL,
	//0x0000900900000000uLL,
	//0xffff000000000000uLL,
	0x0000000000006ff6uLL,// last
};

//static const int sparse = 1;
struct Board
{
	unsigned long long Me, You;
	Board() : Me(0uLL), You(0uLL) {}

	static inline enum State win(const unsigned long long B)
	{
		{//z dir
			unsigned long long b = B & B >> 2 * SIZE * SIZE;
			//static const unsigned long long mask = 0x000000000000ffffuLL;
			if (b & b >> SIZE * SIZE) return State::End;
		}
		{//x dir
			unsigned long long b = B & B >> 2;
			static const unsigned long long mask = 0x1111111111111111uLL;
			if (b & b >> 1 & mask) return State::End;
		}
		{//y dir
			unsigned long long b = B & B >> 2 * SIZE;
			static const unsigned long long mask = 0x000f000f000f000fuLL;
			if (b & b >> SIZE & mask) return State::End;
		}
		{//yz dir
			{
				unsigned long long b = B & B >> 2 * (SIZE * SIZE + SIZE);
				//static const unsigned long long mask = 0x000000000000000fuLL;
				if (b & b >> SIZE * SIZE + SIZE) return State::End;
			}
			{
				unsigned long long b = B & B >> 2 * (SIZE * SIZE - SIZE);
				static const unsigned long long mask = 0x000000000000f000uLL;
				if (b & b >> SIZE * SIZE - SIZE & mask) return State::End;
			}
		}
		{//zx dir
			{
				unsigned long long b = B & B >> 2 * (SIZE * SIZE + 1);
				static const unsigned long long mask = 0x0000000000001111uLL;
				if (b & b >> SIZE * SIZE + 1 & mask) return State::End;
			}
			{
				unsigned long long b = B & B >> 2 * (SIZE * SIZE - 1);
				static const unsigned long long mask = 0x0000000000008888uLL;
				if (b & b >> SIZE * SIZE - 1 & mask) return State::End;
			}
		}
		{//xy dir
			{
				unsigned long long b = B & B >> 2 * (SIZE + 1);
				static const unsigned long long mask = 0x0001000100010001uLL;
				if (b & b >> SIZE + 1 & mask) return State::End;
			}
			{
				unsigned long long b = B & B >> 2 * (SIZE - 1);
				static const unsigned long long mask = 0x0008000800080008uLL;
				if (b & b >> SIZE - 1 & mask) return State::End;
			}
		}
		{//xyz dir
			{
				static const unsigned long long line = BIT(0, 0, 0) | BIT(1, 1, 1) | BIT(2, 2, 2) | BIT(3, 3, 3);
				if ((B & line) == line) return State::End;
			}
			{
				static const unsigned long long line = BIT(0, 0, 3) | BIT(1, 1, 2) | BIT(2, 2, 1) | BIT(3, 3, 0);
				if ((B & line) == line) return State::End;
			}
			{
				static const unsigned long long line = BIT(0, 3, 0) | BIT(1, 2, 1) | BIT(2, 1, 2) | BIT(3, 0, 3);
				if ((B & line) == line) return State::End;
			}
			{
				static const unsigned long long line = BIT(0, 3, 3) | BIT(1, 2, 2) | BIT(2, 1, 1) | BIT(3, 0, 0);
				if ((B & line) == line) return State::End;
			}
		}
		return State::Continue;
	}

	static inline unsigned long long reach(const unsigned long long B)
	{
		unsigned long long ret = 0uLL;
		{//z dir
			const unsigned long long b = B & B >> SIZE * SIZE;
			ret |= B << SIZE * SIZE & b << 3 * SIZE * SIZE;
			ret |= B >> SIZE * SIZE & b << 2 * SIZE * SIZE;
			ret |= b >> SIZE * SIZE & B << SIZE * SIZE;
			ret |= b >> 2 * SIZE * SIZE & B >> SIZE * SIZE;
		}
		{//x dir
			const unsigned long long b = B & B >> 1;
			{
				static const unsigned long long mask = 0x1111111111111111uLL;
				ret |= mask & B >> 1 & b >> 2;
			}
			{
				static const unsigned long long mask = 0x2222222222222222uLL;
				ret |= mask & B << 1 & b >> 1;
			}
			{
				static const unsigned long long mask = 0x4444444444444444uLL;
				ret |= mask & b << 2 & B >> 1;
			}
			{
				static const unsigned long long mask = 0x8888888888888888uLL;
				ret |= mask & b << 3 & B << 1;
			}
		}
		{//y dir
			const unsigned long long b = B & B >> SIZE;
			{
				static const unsigned long long mask = 0x000f000f000f000fuLL;
				ret |= mask & B >> SIZE & b >> 2 * SIZE;
			}
			{
				static const unsigned long long mask = 0x00f000f000f000f0uLL;
				ret |= mask & B << SIZE & b >> SIZE;
			}
			{
				static const unsigned long long mask = 0x0f000f000f000f00uLL;
				ret |= mask & b << 2 * SIZE & B >> SIZE;
			}
			{
				static const unsigned long long mask = 0xf000f000f000f000uLL;
				ret |= mask & b << 3 * SIZE & B << SIZE;
			}
		}
		for (int i = SIZE * SIZE * 3; i < LINES_NUM; i++)
		{
			const unsigned long long b = LINES[i] & ~B;
			if ((b & -b) == b) ret |= b;
		}
		return ret;
	}

	inline enum Cell get_cell(int x, int y, int z) const
	{
		unsigned long long bit = BIT(x, y, z);
		if (Me & bit)return Cell::Me;
		else if (You & bit) return Cell::You;
		else return Cell::None;
	}

	int turn() const
	{
		return __builtin_popcountll(Me | You) + 1;
	}

	enum Color validate() const
	{
		assert(!(Me & You));
		const int d = __builtin_popcountll(Me) - __builtin_popcountll(You);
		assert(d == 0 || d == -1);
		return d == 0 ? Color::Black : Color::White;
	}

	enum Color player() const
	{
		return __builtin_parityll(Me | You) ? Color::White : Color::Black;
	}

	void print() const
	{
		enum Color now = validate();
		unsigned long long black, white;
		if (now == Color::Black) black = Me, white = You;
		else black = You, white = Me;
		cout << "    z=1  z=2  z=3  z=4\n";
		for (int y = SIZE - 1; y >= 0; y--)
		{
			cout << "y=" << y + 1 << " ";
			for (int z = 0; z < SIZE; z++)
			{
				for (int x = 0; x < SIZE; x++)
				{
					const unsigned long long bit = BIT(x, y, z);
					{
						if (black & bit) cout << "\033[31mX\033[m";
						else if (white & bit) cout << "O";
						else cout << "-";
					}
				}
				if (z + 1 < SIZE) cout << " ";
			}
			cout << "\n";
		}
		cout << "  x=1234 1234 1234 1234" << endl;
	}
	
	array<int, LINES_NUM> count() const
	{
		array<int, LINES_NUM> ret;
		for (int i = 0; i < LINES_NUM; i++)
		{
			if ((Me & LINES[i]) && (You & LINES[i])) ret[i] = 0;
			else if (Me & LINES[i]) ret[i] = __builtin_popcountll(Me & LINES[i]);
			else ret[i] = - __builtin_popcountll(You & LINES[i]);
		}
		return ret;
	}
	

	enum State place(int x, int y)
	{
		if (x < 0 || 4 <= x || y < 0 || 4 <= y)
		{
			cout << "out of range : (" << x + 1 << ", " << y + 1 << ")" << endl;
			return State::Invalid;
		}
		int z = 0;
		while (z < 4 && get_cell(x, y, z) != Cell::None) z++;
		if (z == 4)
		{
			cout << "row (" << x + 1 << ", " << y + 1 << ") is full" << endl;
			return State::Invalid;
		}
		Me |= BIT(x, y, z);
		swap(Me, You);
		return win(You);
	}

	enum State place_fast(unsigned long long bit)
	{
		Me |= bit;
		swap(Me, You);
		return win(You);
	}

	unsigned long long valid_move() const
	{
		return ((Me | You) << SIZE * SIZE | ((1uLL << SIZE * SIZE) - 1)) & ~(Me | You);
	}

	Board place_fast_clone(unsigned long long bit) const
	{
		Board b = *this;
		b.Me |= bit;
		swap(b.Me, b.You);
		return b;
	}
};


struct Player
{
	bool verbose;
	int random;
	Player() : verbose(false), random(0) {}
	void set_verbose(bool verbose_) {verbose = verbose_;}
	void set_random(int random_) {random = random_;}
	pair<int, int> move_random(Board board)
	{
		unsigned long long hand = board.valid_move();
		assert(hand);

		const int sz = __builtin_popcountll(hand);
		int idx = rng() % sz + 1;
		int v = 0;
		for (; v < 64; v++) if (hand >> v & 1)
		{
			idx--;
			if (idx == 0) break;
		}
		assert(idx == 0);

		if (verbose) cout << "random move (" << X(v) + 1 << ", " << Y(v) + 1 << ", " << Z(v) + 1 << ")" << endl;
		return make_pair(X(v), Y(v));
	}
	virtual pair<int, int> move(Board board) = 0;

};

struct HumanPlayer : Player
{
	pair<int, int> move(Board board) override
	{
		enum Color now = board.validate();
		cout << endl;
		board.print();
		cout << "Input x y: place ";
		if (now == Color::Black) cout << "\033[31mX\033[m";
		else cout << "O";
		cout << " to (x, y)\t(1 <= x, y <= 4)"<<endl;
		int x, y;
		cin >> x >> y;
		return make_pair(x - 1, y - 1);
	}
};

// thread_local int evaluatesfir_tmp[33] = {};
// thread_local int evaluatessec_tmp[33] = {};
// thread_local int record_tmp[64] = {};


struct Game
{
	Board board;
	Player* player1;
	Player* player2;
	vector<pair<int, int> > hand, start;
	bool verbose;

	int evaluatesfir_tmp[33] = {};
	int evaluatessec_tmp[33] = {};
	int record_tmp[64] = {};

	Game(Player* p1, Player* p2, bool verbose=false, vector<pair<int, int> > start = {}) : player1(p1), player2(p2), verbose(verbose), start(start)
	{
		p1 -> set_verbose(verbose);
		p2 -> set_verbose(verbose);

		for(int i = 0; i < 33; i++)evaluatesfir_tmp[i] = INF;
		for(int i = 0; i < 33; i++)evaluatessec_tmp[i] = INF;
		for(int i = 0; i < 64; i++)record_tmp[i] = INF;
	}

	enum State move(int turn)
	{
		enum State ret;
		while (true)
		{
			auto st = chrono::system_clock::now();
			pair<int, int> xy;
			if (turn < start.size()) xy = start[turn];
			else
			{
				Player* current_player = (turn % 2 == 0) ? player1 : player2;
				xy = current_player -> move(board);
			}
			ret = board.place(xy.first, xy.second);
			if (ret == State::Invalid)
			{
				cout << "Invalid move : (" << xy.first + 1 << ", " << xy.second + 1 << ")" << endl;
				continue;
			}
			hand.push_back(xy);
			if (verbose)
			{
				auto msec = chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - st);
				cout << "[turn " << turn + 1 << "] Place to (" << xy.first + 1 << ", " << xy.second + 1 << ") ";
				if (turn % 2 == 0) cout << "\033[31m(Black)\033[m";
				else cout << "(White)";
				cout << " by " << msec.count() / 1e3 << " sec" << endl;
				board.print();
			}
			break;
		}
		return ret;
	}

	enum Color game()
	{
		enum State ret;
		for (int turn = 0; turn < BOARD_SIZE; turn++)
		{
			ret = move(turn);
			if (ret == State::End) break;
		}
		board.print();
		enum Color result;
		if (ret == State::End)
		{
			evaluatesfir_tmp[0] = 1;
			evaluatessec_tmp[0] = -1;
			cout << "END : winner is ";
			if (board.validate() == Color::White)
			{
				result = Color::Black;
				cout << "\033[31mBlack\033[m";
			}
			else
			{
				evaluatesfir_tmp[0] = -1;
				evaluatessec_tmp[0] = 1;
				result = Color::White;
				cout << "White";
			}
			cout << " by " << hand.size() << " moves" << endl;
		}
		else
		{
			evaluatesfir_tmp[0] = 0;
			evaluatessec_tmp[0] = 0;
			result = Color::Draw;
			cout << "DRAW" << endl;
		}

		int j = 0;
		for(auto [x,y] : hand)
		{
			record_tmp[j] = 4*(3 - y) + x;
			j++;
		}
		{
			cout << "hands :";
			int j = 0;
			for (auto [x,y] : hand)cout << " {" << x << ", " << y << "},";
			cout << endl;
			cout << "\033[31mfirst (vic or def, evaluates...)\033[m : ";
			cout<<"\033[31m";
			for(int i = 0; i < 33; i++) cout << evaluatesfir_tmp[i] << "," ;
			cout<<"\033[m";
			cout << endl;
			cout << "second (vic or def, evaluates...) : ";
			for (int i = 0 ; i<33 ; i++) cout << evaluatessec_tmp[i] << ",";
			cout << endl;
			cout << "record";
			for(int i = 0; i < 64; i++) cout << record_tmp[i] << ",";
            cout << endl;
		}
		
		return result;
	}
};


template<typename F>
pair<unsigned long long, int> read_DFS(Board board, int level, const F& evaluate_func)
{
	if (level == 0) return make_pair(0uLL, evaluate_func(board));

	assert(level >= 1);
	unsigned long long hand = board.valid_move();
	if (!hand) return make_pair(0uLL, 0);

	{//reach
		{
			const unsigned long long r = hand & Board::reach(board.Me);
			if (r) return make_pair(r, INF);
		}
		{
			const unsigned long long r = hand & Board::reach(board.You);
			if (r) hand = r;
		}
	}

	pair<unsigned long long, int> mv = make_pair(0uLL, -INF);
	while (hand)
	{
		const unsigned long long bit = hand & -hand;
		Board b = board;
		enum State r = b.place_fast(bit);

		assert(r == State::Continue);
		int ev = -read_DFS(b, level - 1, evaluate_func).second;
		if (ev > INF - BOARD_SIZE * 2) ev = ev - 1;
		else if (ev < -INF + BOARD_SIZE * 2) ev = ev + 1;
		if (mv.second < ev) mv = make_pair(0uLL, ev);
		if (mv.second == ev) mv.first |= bit;
		hand ^= bit;
	}
	assert(mv.first);
	return mv;
}



template<typename F>
struct AIPlayer : Player
{
	using Player::verbose;
	using Player::random;
	int level;
	F evaluate_func;
	Game* game = nullptr;
	AIPlayer(int level, F evaluate_func) : level(level), evaluate_func(evaluate_func) { assert(level >= 1); }

	void set_game(Game* g) { game = g; }

	//int evaluate_board(Board board, int level, int alpha, int beta)
	int evaluate_board(Board board, int level, int alpha, int beta)
	{
		unsigned long long hand = board.valid_move();
		if (!hand) return 0;

		{//reach
			{
				const unsigned long long r = hand & Board::reach(board.Me);
				if (r) return INF - board.turn();
			}
			{
				const unsigned long long r = Board::reach(board.You);
				if (hand & r) hand = hand & r;
				else if (level <= 0) return evaluate_func(board);
				hand &= ~(r >> SIZE * SIZE);
				if (!hand) return -(INF - (board.turn() + 1));
			}
		}

		const int turn = board.turn();// turn number (the number of stones = turn - 1)
        //if(level >= 8 && turn < 27 || level >= 10 && turn >= 27)
		if(level >= 8 && turn < 46 || level >= 10 && turn >= 46)//教師データ取得用
        {
            //4手読みパート
            vector<pair<int, unsigned long long>> dynamic;
            for (const unsigned long long mask: move_order)
            {
                unsigned long long h = hand & mask;
                hand ^= h;
                while (h)
                {
                    const unsigned long long bit = h & -h;
                    // Board b = board;
                    // enum State r = b.place_fast(bit);

					Board b = board.place_fast_clone(bit);
					enum State r = Board::win(b.You);

                    assert(r == State::Continue);
                    int ev = -evaluate_board(b, 3 , -INF , INF);
                    dynamic.push_back(make_pair(ev, bit));
                    h ^= bit;
                }
            }
            sort(dynamic.rbegin(), dynamic.rend());

            for(int i = 0; i < dynamic.size(); i++)
            {
                {
                    const unsigned long long bit = dynamic[i].second;
                    // Board b = board;
                    // enum State r = b.place_fast(bit);

					Board b = board.place_fast_clone(bit);
					enum State r = Board::win(b.You);

                    assert(r == State::Continue);
                    int ev = -evaluate_board(b, level - 1, -beta, -alpha);
                    alpha = max(alpha, ev);
                    if (alpha >= beta) break;
                }
                if (alpha >= beta) break;
            }
        }
        else
        {
            for(const unsigned long long mask: move_order)
            {
                unsigned long long h = hand & mask;
                hand ^= h;
                while (h)
                {
                    const unsigned long long bit = h & -h;
                    //const unsigned long long bit = dynamic[i].second;
                    // Board b = board;
                    // enum State r = b.place_fast(bit);

					Board b = board.place_fast_clone(bit);
					enum State r = Board::win(b.You);

                    assert(r == State::Continue);
                    int ev = -evaluate_board(b, level - 1, -beta, -alpha);
                    alpha = max(alpha, ev);
                    if (alpha >= beta) break;
                    h ^= bit;
                }
                if (alpha >= beta) break;
            }
        }
		return alpha;
	}

	pair<int, int> move(Board board) override
	{
		unsigned long long hand = board.valid_move();
		assert(hand);

		{
			const unsigned long long r = hand & Board::reach(board.Me);
			if (r)
			{
				int v = __builtin_ctzll(r);
				return make_pair(X(v), Y(v));
			}
		}
		{
			const unsigned long long r = hand & Board::reach(board.You);
			if (r) hand = r;
		}

		if (random && rng() % 100 < random) return move_random(board);

		unsigned long long mv = 0uLL;
		int mx = -INF;
		const int turn = board.turn();// turn number (the number of stones = turn - 1)

        //4手読みパート
        vector<pair<int, unsigned long long>> dynamic;
        for (const unsigned long long mask: move_order)
		{
            unsigned long long h = hand & mask;
			hand ^= h;
			while (h)
			{
				const unsigned long long bit = h & -h;
				// Board b = board;
				// enum State r = b.place_fast(bit);

				Board b = board.place_fast_clone(bit);
				enum State r = Board::win(b.You);

				assert(r == State::Continue);
				int ev = -evaluate_board(b, 3 , -INF , INF);
				//if (mx < ev) mv = 0uLL, mx = ev;
				//if (mx == ev) mv |= bit;
                dynamic.push_back(make_pair(ev, bit));
				h ^= bit;
			}
		}
        sort(dynamic.rbegin(), dynamic.rend());

	    //while (hand)//
        for(int i = 0; i < dynamic.size(); i++)
        //for (const unsigned long long mask: move_order)
		{
            //unsigned long long h = hand & mask;
            //unsigned long long h = hand & dynamic[i].second;
			//hand ^= h;
			//while (h)
			{
				//const unsigned long long bit = h & -h;
                const unsigned long long bit = dynamic[i].second;
				// Board b = board;
				// enum State r = b.place_fast(bit);

				Board b = board.place_fast_clone(bit);
				enum State r = Board::win(b.You);

				assert(r == State::Continue);//下の行について、?nはn+1手読みturn>=45?19:turn>=43?15:turn>=41?13:turn>=39?11:turn>=33?9
				// int ev = -evaluate_board(b,level - 1, -INF , -mx + 1);
				// int ev = -evaluate_board(b,turn>=46?14:turn>=39?9:level - 1, -INF , -mx + 1);
				// int ev = -evaluate_board(b,turn>=44?16::level - 1, -INF , -mx + 1);//教師データ用ver3
                // int ev = -evaluate_board(b,turn>=43?17:turn>=35?9:level - 1, -INF , -mx + 1);//教師データ用ver4,シグモイド関数の係数は920
				// int ev = -evaluate_board(b,turn>=40?21:turn>=33?11:turn>=25?9:level - 1, -INF , -mx + 1);//教師データ用ver5,シグモイド関数の係数は1840
				// int ev = -evaluate_board(b,turn>=39?22:turn>=31?11:turn>=23?9:level - 1, -INF , -mx + 1);//教師データ用ver6,シグモイド関数の係数は1840
                // int ev = -evaluate_board(b,turn>=39?21:turn>=37?12:turn>=29?10:level - 1, -INF , -mx + 1);//この行で途中からの読み手数を変更できる
				// int ev = -evaluate_board(b,turn>=39?21:turn>35?13:turn>=29?11:level - 1, -INF , -mx + 1);//この行で途中からの読み手数を変更できる
				int ev = -evaluate_board(b,turn>=39?22:turn>=33?13:turn>=21?11:level - 1, -INF , -mx + 1);//この行で途中からの読み手数を変更できる
				if (mx < ev) mv = 0uLL, mx = ev;
				if (mx == ev) mv |= bit;
				//h ^= bit;
			}
		}

		//const int turn = board.turn();// turn number (the number of stones = turn - 1)
		//auto[mv, ev] = read_DFS(board, turn >= 40 ? level : level, evaluate_func);
		enum Color now = board.validate();
		if(turn % 2 == 0)
		{
			//evaluatessec[game_count][turn >> 1] = mx;
			game->evaluatessec_tmp[turn / 2] = mx;
			//cout << "score game_count = " << game_count << endl;
		}
		else
		{
			//evaluatesfir[game_count][turn + 1 >> 1] = mx;
			game->evaluatesfir_tmp[(turn + 1) / 2] = mx;
		}
		if (verbose)
		{
			if (now == Color::Black) cout<<"\033[31m";
			cout << "score = " << mx << " by";
			for (int xyz = 0; xyz < BOARD_SIZE; xyz++) if (mv & 1uLL << xyz)
			{
				cout << " (" << X(xyz) + 1 << ", " << Y(xyz) + 1 << ", " << Z(xyz) + 1 << "),";
			}
			cout << endl;
			cout << "win : " << 100 / (1 + exp((double)-mx / 400)) << " %";
			if (now == Color::Black) cout<<"\033[m";
			cout<<endl;
		}
		assert(mv);
		{
			vector<pair<int, int> > XY;
			for (int xyz = 0; xyz < BOARD_SIZE; xyz++) if (mv & 1uLL << xyz) XY.emplace_back(X(xyz), Y(xyz));
			return XY[rng() % XY.size()];
		}
	}
};

template<typename F>
struct AIPlayer_minimax : Player
{
	using Player::verbose;
	int level;
	F evaluate_func;
	AIPlayer_minimax(int level, F evaluate_func) : level(level), evaluate_func(evaluate_func) { assert(level >= 1); }

	pair<int, int> move(Board board) override
	{
		unsigned long long hand = board.valid_move();
		assert(hand);

		{
			const unsigned long long r = hand & Board::reach(board.Me);
			if (r)
			{
				int v = __builtin_ctzll(r);
				return make_pair(X(v), Y(v));
			}
		}
		{
			const unsigned long long r = hand & Board::reach(board.You);
			if (r)
			{
				int v = __builtin_ctzll(r);
				return make_pair(X(v), Y(v));
			}
		}

		const int turn = board.turn();// turn number (the number of stones = turn - 1)
		auto[mv, ev] = read_DFS(board, turn >= 40 ? level : level, evaluate_func);
		enum Color now = board.validate();
		if (verbose)
		{
			if (now == Color::Black) cout<<"\033[31m";
			cout << "score = " << ev << " by";
			for (int xyz = 0; xyz < BOARD_SIZE; xyz++) if (mv & 1uLL << xyz)
			{
				cout << " (" << X(xyz) + 1 << ", " << Y(xyz) + 1 << ", " << Z(xyz) + 1 << "),";
			}
			if (now == Color::Black) cout<<"\033[m";
			cout<<endl;
		}
		assert(mv);
		{
			vector<pair<int, int> > XY;
			for (int xyz = 0; xyz < BOARD_SIZE; xyz++) if (mv & 1uLL << xyz) XY.emplace_back(X(xyz), Y(xyz));
			return XY[rng() % XY.size()];
		}
	}
};

//90度回転変換
vector<vector<int>> rotate90(vector<vector<int>> record){
	vector<vector<int>> rotated;
	for(int i = 0; i < record.size(); i++){
		//初手から回転させていく
		int x = record[i][0];
		int y = record[i][1];
		
		rotated.push_back({3-y,x});
	}
	return rotated;
}
//鏡映変換,y軸と平行に折り返す
vector<vector<int>> reflection(vector<vector<int>> record){
	vector<vector<int>> reflected;
	for(int i = 0; i < record.size(); i++){
		//初手から順に鏡映させていく
		int x = record[i][0];
		int y = record[i][1];

		reflected.push_back({3-x,y});
	}
	return reflected;
}

Board encode_to_board(const vector<vector<int>>& record) {
    Board board;
    for (auto& h : record) {
        enum State st = board.place(h[0], h[1]);
        assert(st != State::Invalid);
        assert(st != State::End); //4手目までに勝敗はつかない
    }
    return board;
}
//Board比較関数
bool same_board(const Board& a, const Board& b) {
    return a.Me == b.Me && a.You == b.You;
}


int main(){
    vector<vector<vector<int>>> openings;

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){

            for(int k = 0; k < SIZE; k++){
                for(int l = 0; l < SIZE; l++){

                    for(int m = 0; m < SIZE; m++){
                        for(int n = 0; n < SIZE; n++){

                            for(int p = 0; p < SIZE; p++){
                                for(int q = 0; q < SIZE; q++){
                                    //初手
                                    vector<int> hand_0 = {i,j};
                                    //2手目
                                    vector<int> hand_1 = {k,l};
                                    //3手目
                                    vector<int> hand_2 = {m,n};
                                    //4手目
                                    vector<int> hand_3 = {p,q};

                                    openings.push_back({hand_0, hand_1, hand_2, hand_3});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
	/*
	//デバッグ
    vector<vector<int>> rotate;
	rotate = rotate90(openings[1]);
	for(int i = 0; i < 4; i++){
		cout << rotate[i][0]  << " "<< rotate[i][1] << endl;
	}
	
	vector<vector<int>> ref;
	ref = reflection(openings[1]);
	for(int i = 0; i < 4; i++){
		cout << ref[i][0]  << " "<< ref[i][1] << endl;
	}
	*/
	vector<bool> judge(openings.size(),true);
	for(int i = 0; i < openings.size();i++){
		//もしjudge[i] == false ならもうその同型類は消し終わっている
		if(!judge[i]) continue;
		//record_0は比較元の局面
		vector<vector<int>> record_0 = openings[i];
		Board board_0 = encode_to_board(record_0);

		vector<vector<int>> record_90 = rotate90(record_0);
		Board board90 = encode_to_board(record_90);

		vector<vector<int>> record_180 = rotate90(rotate90(record_0));
		Board board180 = encode_to_board(record_180);

		vector<vector<int>> record_270 = rotate90(rotate90(rotate90(record_0)));
		Board board270 = encode_to_board(record_270);

		vector<vector<int>> reflected = reflection(record_0);
		Board board_ref = encode_to_board(reflected);

		vector<vector<int>> reflected90 = reflection(record_90);
		Board board_ref_90 = encode_to_board(reflected90);

		vector<vector<int>> reflected180 = reflection(record_180);
		Board board_ref_180 = encode_to_board(reflected180);

		vector<vector<int>> reflected270 = reflection(record_270);
		Board board_ref_270 = encode_to_board(reflected270);

		for(int j = i+1; j < openings.size(); j++){
			//j < iでopenings[j]を消す必要があったとしても,その配列はi がjだった時点の探索で現在のopenings[i]が消されている
			//if(j == i) continue;
			if(!judge[j]) continue;
			vector<vector<int>> record_com = openings[j];
			Board board_com = encode_to_board(record_com);
			//board_comをboard_0の変換たちと比較
			if(same_board(board_com,board_0) || same_board(board_com,board90) || same_board(board_com, board180) || same_board(board_com, board270)
			|| same_board(board_com, board_ref) || same_board(board_com, board_ref_90) || same_board(board_com, board_ref_180) || same_board(board_com,board_ref_270)){
				judge[j] = false;
			} 
		}
	}
	//uniqueに対称性で割った代表元を集める
	vector<vector<vector<int>>> unique;
	for(int i = 0; i < openings.size(); i++){
		if(judge[i]){
			unique.push_back(openings[i]);
		}
	}
	//出力
	ofstream ofs("unique_openings.txt");

	ofs << "vector<vector<vector<int>>> unique = {\n";

	for (const auto& opening : unique) {
		ofs << "    {";
		for (int i = 0; i < opening.size(); i++) {
			ofs << "{" << opening[i][0] << "," << opening[i][1] << "}";
			if (i + 1 != opening.size()) ofs << ", ";
		}
		ofs << "},\n";
	}

	ofs << "};\n";
	ofs.close();

}
