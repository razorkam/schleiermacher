#include <string>
#include <bitset>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cctype>
#include <stack>
#include <iomanip>
//#include <boost/filesystem.hpp>




using std::string;
using std::bitset;
using std::max;
using std::ifstream;
using std::cout;
using std::cerr;
using std::vector;

//namespace bfs = boost::filesystem;

enum token_type {EMPTY, IDENTIFIER , KEYWORD,  OPERATOR , PUNCTUATOR , LITERAL, NUMBER};   //token types // glob
const char* token_name[] = {"EMPTY", "IDENTIFIER", "KEYWORD",  "OPERATOR", "PUNCTUATOR", "LITERAL", "NUMBER"};   //token types // glob
const int GAP_SCORE = -8;

 struct alignment

 {

   bitset<3> both;
   bitset<3> gap_x;
   bitset<3> gap_y;

   alignment()
   {
     both = 1;
     gap_x = 2;
     gap_y = 4;
   }

 }align_type;

 struct tokens
 {

    string token;
    token_type type;

 }token;

  class string_align
{
    private:
    int nGap_score;
    int id_matchScore,id_misScore,kw_matchScore,kw_misScore,op_matchScore,op_misScore,lit_matchScore,lit_misScore,num_matchScore,num_misScore;
    int default_misScore;
    std::vector<string> str_x,str_y;
    std::vector<token_type> types_x, types_y;

    public:
    std::stack<string> str_x_aligned,str_y_aligned,str_match;
	std::stack<tokens> token_x_aligned,token_y_aligned;

    public:
    string_align (std::vector<string> x, std::vector<token_type> x_types, std::vector<string> y, std::vector<token_type> y_types)
     {
        nGap_score = GAP_SCORE;
        id_matchScore = 6;
        id_misScore = 4;
        kw_matchScore = 10;       //mis(match) score;
        kw_misScore = -10;
        op_matchScore = 8;
        op_misScore = -10;
        lit_matchScore = 7;
        lit_misScore = 5;
        num_matchScore = 7;
        num_misScore = 0;
        default_misScore = -10;
        str_x = x;
        types_x = x_types;
        str_y = y;
        types_y = y_types;
     }

    private:

    bool has_flag(bitset<3> event, bitset<3> flag)
    {
        bitset<3> test = event & flag;
        if (test.count() != 0) return true;
        return false;
    }

   public:

    int match_score (string x, token_type type_x , string y, token_type type_y)

    {
        if (type_x == type_y)
        {
            if (type_x == IDENTIFIER)
            {
                if (x==y) return id_matchScore;
                return id_misScore;
            }

            if (type_x == KEYWORD)
            {
                if (x==y) return kw_matchScore;
                return kw_misScore;
            }

            if (type_x == OPERATOR || type_x == PUNCTUATOR)
            {
                if (x==y) return op_matchScore;
                return op_misScore;
            }

            if (type_x == LITERAL)
            {
                if (x==y) return lit_matchScore;
                return lit_misScore;
            }

            if (type_x == NUMBER)
            {
                if (x==y) return num_matchScore;
                return num_misScore;
            }
        }
        return default_misScore;
    }



   int align()

    {
      int** scores = new int*[str_x.size()+1];
      for(size_t i = 0; i < str_x.size()+1; ++i)
      scores[i] = new int[str_y.size()+1];

      scores[0][0] = 0;

      bitset<3>** events = new bitset<3>*[str_x.size()+1];
      for(size_t i = 0; i < str_x.size()+1; ++i)
      events[i] = new bitset<3>[str_y.size()+1];



      unsigned int i,j;

      for (i = 1; i <= str_x.size(); i++)
			{
				scores[i][0] = scores[i - 1][0] + nGap_score;
				events[i][0] = align_type.gap_y;
			}

      for (j = 1; j <= str_y.size(); j++)
			{
				scores[0][j] = scores[0][j - 1] + nGap_score;
				events[0][j] = align_type.gap_x;
			}


	  for (i = 1; i <= str_x.size(); i++)
	      for (j = 1; j <= str_y.size(); j++)
	  		{

	          int m = scores[i-1][j-1] + match_score(str_x.at(i-1),types_x.at(i-1),str_y.at(j-1),types_y.at(j-1));
	          int g1 = scores[i-1][j] + nGap_score;
	          int g2 = scores[i][j-1] + nGap_score;
	          scores[i][j] = max(m,max(g1,g2));
	          bitset<3> type = align_type.both;
	          if (scores[i][j] == g1) type |= align_type.gap_y;
	          if (scores[i][j] == g2) type |= align_type.gap_x;
	          if (scores[i][j] > m ) type ^= align_type.both;
	          events[i][j] = type;


            }

       i = str_x.size();
       j = str_y.size();

      while (i > 0 || j > 0)
			{
				if (has_flag(events[i][j],align_type.both) && i > 0 && j > 0)
				{
					str_x_aligned.push(str_x[--i]);
					str_y_aligned.push(str_y[--j]);
				    str_match.push(str_x[i]==str_y[j] ? "=" : "!");
					tokens token_x = { str_x[i],types_x[i] },token_y = { str_y[j],types_y[j] };
					token_x_aligned.push(token_x);
					token_y_aligned.push(token_y);
				}
				else if (has_flag(events[i][j],align_type.gap_y) && i > 0)
				{
					str_x_aligned.push(str_x[--i]);
					str_y_aligned.push(".");
					str_match.push("^");
					tokens token_x = { str_x[i],types_x[i] },token_y = { "",EMPTY };
					token_x_aligned.push(token_x);
					token_y_aligned.push(token_y);
				}
				else if (has_flag(events[i][j],align_type.gap_x) && j > 0)
				{
					str_x_aligned.push(".");
					str_y_aligned.push(str_y[--j]);
					str_match.push("v");
					tokens token_x = { "",EMPTY },token_y = { str_y[j],types_y[j] };
					token_x_aligned.push(token_x);
					token_y_aligned.push(token_y);
				}

            }


     return scores[str_x.size()][str_y.size()];
      }
  };


class parsing
{
    public:
    parsing (std::vector<string> kw)
    {
       kw_list = kw;
    }


    private:
    std::vector<string> kw_list;
    string entire;
    std::string::iterator iter;


    public:
    std::vector<string> get_parsing()
    {
        return tokens;
    }

    public:
    std::vector<token_type> get_types()
    {
        return types;
    }



    public:
    int init (ifstream &f)
          {
            string extracted;
            while (std::getline(f,extracted))
                {
                        for (iter = extracted.begin(); iter != extracted.end(); iter++)    // corr. testing
                    {

                              if (*iter == '"')
                         {
                            iter++;
                            if (iter == extracted.end()) return -1;
                            while (*iter != '"')
                            {
                              iter++;
                              if (iter == extracted.end()) return -1;
                            }
                         }
                           if (*iter == '\'')
                         {
                            iter++;
                            if (iter == extracted.end()) return -1;
                            while (*iter != '\'')
                            {
                              iter++;
                              if (iter == extracted.end()) return -1;
                            }
                         }

                         if (*iter == '/' && *(iter+1) == '/')
                         {
                            extracted.erase(iter, extracted.end());
                            break;
                         }

                    }
                    entire += extracted;
                    extracted.clear();
                }

                for (iter = entire.begin(); iter != entire.end(); iter++)
                {
                    if (*iter == '"')
                    {
                        iter++;
                        while (*iter != '"') iter++;
                    }

                    if (*iter == '\'')
                    {
                        iter++;
                        while (*iter != '\'') iter++;
                    }

                    if (*iter == '/' && *(iter+1) == '*')
                    {
                        std::string::iterator tmp = iter;
                        iter += 2;
                        while ((*iter != '*' || *(iter+1) != '/')  && iter != entire.end())
                        {
                          iter++;
                        }
                        if (iter == entire.end())
                        {
                            entire.erase(tmp,entire.end());
                            break;
                        }
                        iter += 2;
                        iter = entire.erase(tmp,iter);
                    }
                }

            return 0;
          }


    public:
    void str_format ()
    {

       for (iter = entire.begin(); iter != entire.end(); iter++)     //deleting exc. ws
             if (isspace(*iter))
                if ((!is_idensym(*(iter+1)) || !is_idensym(*(iter-1))) && (!is_punsym((iter+1)) || !is_punsym((iter-1)))   )
                {
                    iter = entire.erase(iter) - 1;
                }
       for (iter = entire.begin(); iter != entire.end(); iter++)   //add req.
       {
          if (is_idensym(*iter))
          {

            switch (*(iter-1))
            {
                case '-':
                if (*(iter-2) == '-') iter = entire.insert(iter-2,' ') + 3;
                break;
                case '+':
                if (*(iter-2) == '+') iter = entire.insert(iter-2,' ') + 3;
                break;
            }


            if (*(iter-1) == '!' || *(iter-1) == '~')
             {
                if (*(iter-2) == '+' || *(iter-2) == '-')
                    if (*(iter-3) == '+' || *(iter-3) == '-')
                    iter = entire.insert(iter-3,' ') + 4;
                iter = entire.insert(iter-1,' ') + 2;
             }


             if (*(iter-1) == '*')
             {
                int k = 1;
                iter--;
                while (*iter == '*')
                {
                    iter--;
                    k++;
                }
                iter++;
                if (*(iter-1) == '+' || *(iter-1) == '-')
                    if (*(iter-2) == '+' || *(iter-2) == '-')
                      iter = entire.insert(iter-2,' ') + 3;
                iter = entire.insert(iter,' ') + k;
             }
            if (*(iter-1) == '&')
             {
                int k = 1;
                iter--;
                while (*iter == '&')
                {
                    iter--;
                    k++;
                }
                iter++;
                if (*(iter-1) == '+' || *(iter-1) == '-')
                    if (*(iter-2) == '+' || *(iter-2) == '-')
                      iter = entire.insert(iter-2,' ') + 3;
                iter = entire.insert(iter,' ') + k;
             }

              switch (*(iter+1))
            {
                case '-':
                if (*(iter+2) == '-') iter = entire.insert(iter+3,' ') - 3;
                break;
                case '+':
                if (*(iter+2) == '+') iter = entire.insert(iter+3,' ') - 3;
                break;

            }

           if  (*(iter+1) == '*')
            {
                int k = 1;
                iter++;
                while (*iter == '*')
                {
                    iter++;
                    k++;
                }
                iter = entire.insert(iter,' ') - k;
            }
             if  (*(iter+1) == '&')
            {
                int k = 1;
                iter++;
                while (*iter == '&')
                {
                    iter++;
                    k++;
                }
                iter = entire.insert(iter,' ') - k;
            }
        }
          switch (*iter)
          {
            case '(':case ')': case '[': case ']': case '{': case '}': case '"': case '\'': case '#':
            iter = entire.insert(iter+1,' ') - 1;
            iter = entire.insert(iter,' ') + 1;
            break;
            case '.':
            if (!isdigit(*(iter-1))) iter = entire.insert(iter,' ') + 1;
            break;
          }

       }
    }

    private:      //inspector functions

    bool is_idensym(char c)
    {
      if (isalnum(c) || c == '_' || c == '$') return true;
      return false;
    }

    bool is_punsym(std::string::iterator iter)
    {
      if (*iter == '.' && isdigit(*(iter+1))) return false;
      static string pun_sym = "(){}+-*/=<>.,;:[]%!&|~^#'\"?";
      if (pun_sym.find(*iter) != std::string::npos) return true;
      return false;
    }

    bool is_keyword (string token)

    {
      for (std::vector<string>::iterator iter = kw_list.begin(); iter != kw_list.end(); iter ++)
      {
        if (token == *iter) return true;
      }

      return false;
    }

      bool is_numsym (std::string::iterator iter)

    {
      static string num_spsym = "ex-.ABCDEF";
      if (*iter == '.')
      {
        if (isdigit(*(iter+1))) return true;
        return false;
      }
      if (*iter == '-')
      {

        if (*(iter-1) == 'e' || *(iter-1) == 'E') return true;
        return false;

      }
      if (isdigit(*iter) || num_spsym.find(*iter) != std::string::npos) return true;
      return false;
    }



    private:
    vector<string> tokens;
    vector<token_type> types;
    string curr_tok;

    public:
    void dismember()
    {

      for (iter = entire.begin();iter != entire.end();)
      {

        if (isalpha(*iter) || (*iter) == '_' || (*iter == '$'))
        {

          while (is_idensym(*iter))
           {
                curr_tok += *iter;
                iter++;
           }

          tokens.push_back(curr_tok);

          if (is_keyword(curr_tok))
          {
           types.push_back(KEYWORD);
          }
          else
          {
            types.push_back(IDENTIFIER);
          }
          curr_tok.clear();
        } else
       if (is_punsym(iter))
       {
         int flag = 0;
             if (*iter == '"')
             {
                curr_tok += *iter++;
                while (*iter != '"')
                {
                    curr_tok += *iter++;
                }
                curr_tok += *iter++;
                tokens.push_back(curr_tok);
                types.push_back(LITERAL);
                curr_tok.clear();
                flag = 1;
             }
         if (!flag)
         {
             if (*iter == '\'')
             {
                curr_tok += *iter++;
                while (*iter != '\'')
                {
                    curr_tok += *iter++;
                }
                curr_tok += *iter++;
                tokens.push_back(curr_tok);
                types.push_back(LITERAL);
                curr_tok.clear();
                flag = 1;
             }
         }
         if (!flag)
         {
             while (is_punsym(iter))
             {
               curr_tok += *iter;
               iter++;
             }
             tokens.push_back(curr_tok);
             if (curr_tok == "#" || curr_tok == ";") types.push_back(PUNCTUATOR); else types.push_back(OPERATOR);
             curr_tok.clear();
         }
        } else

       if (isdigit(*iter) || *iter == '.')
       {
         while (is_numsym(iter))
         {
            curr_tok += *iter;
            iter++;
         }
         tokens.push_back(curr_tok);
         types.push_back(NUMBER);
         curr_tok.clear();
       } else
         iter++;
     }
   }


  };




int main (int argc, const char **argv)
{
ifstream kw_list;
string extracted;
kw_list.open("keywords_reserved.txt");
std::vector<string> kw;
while (std::getline(kw_list,extracted)) kw.push_back(extracted);

if (argc < 3)
    {
        cerr << "File isn't specified";
        return -1;
    }


std::vector<string> file1_parsed,file2_parsed;
std::vector<token_type> file1_types,file2_types;
ifstream f1,f2;
f1.open(argv[1]);
f2.open(argv[2]);

parsing file1 (kw);
parsing file2 (kw);
if (file1.init(f1) || file2.init(f2))
{
    cerr << "Some of files are unsafe for parsing";
    return -1;
}

file1.str_format();
file2.str_format();

file1.dismember();
file2.dismember();
file1_parsed = file1.get_parsing();
file2_parsed = file2.get_parsing();
file1_types = file1.get_types();
file2_types = file2.get_types();
string_align alignment(file1_parsed,file1_types,file2_parsed, file2_types);
int scores = alignment.align();




int amount = 0;

//std::stack<string> *xa = &alignment.str_x_aligned,*ya = &alignment.str_y_aligned;
std::stack<tokens> *xa = &alignment.token_x_aligned,*ya = &alignment.token_y_aligned;

while(!xa->empty() && !ya->empty())
{
//if(!xa->empty())
//{
	cout << xa->top().token << '\t' << ya->top().token << '\t'
		<< (xa->top().type != EMPTY && ya->top().type != EMPTY
			? alignment.match_score(xa->top().token,xa->top().type,ya->top().token,ya->top().type)
			: GAP_SCORE)
		<< '/' << token_name[xa->top().type] << '/' << token_name[ya->top().type]
		<< std::endl;

	xa->pop();
	ya->pop();
	amount++;
}

cout << "Score: " << scores << std::endl << "Amount: " << amount << std::endl;
double plag = ((double)scores) / (6*amount); // - 0.25;    //empirically
if (plag < 0) plag = 0;
if (plag > 1) plag = 1;
double uniqueness = 1-plag;
cout << "Uniqueness:" << uniqueness;










}


