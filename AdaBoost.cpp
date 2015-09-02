#include<iostream>
#include<math.h>
#include<vector>
#include<stdlib.h>
#include<string>
#include <fstream>
using namespace std;

#define pos 10
#define neg 10
#define T 4

//グローバル変数
double D[pos+neg];		//確率分布
double D0[pos+neg];		//確率分布の初期状態
vector<double> alpha;

//関数
int hypothesis(int x_pos,int x_neg,int x);	//h(x)=sgn[d(x_neg,x)-x(x_pos,x)]	(xの扱い方に注意　順番がpos_1,pos_2,・・・,pos_pos,neg_1,neg_2,・・・,neg_neg)
double yuido(int x_pos,int x_neg);
int hypothesis_test(int x_pos,int x_neg);
int sign(double x);

//構造体
typedef struct
{
	double x1;
	double x2;
	int y;
} exsample;
exsample positive[pos];
exsample negative[neg];
exsample test;

typedef struct
{
	int pos_number;
	int neg_number;
} function;
vector<function>f;

int main(){
	int i,j;
	int t;
	int sample = pos+neg;	//学習データの総数
	double ganma;			//優位度γ
	int max_p;				//優位度γが最大になるx_p
	int max_n;				//優位度γが最大になるx_n
	double max_ganma;		//最大優位度
	function max;			//統合仮説への追加時の一時変数
	double beta;			//重みαを計算するためのβ
	double temp;			//wを計算するために使う一時変数
	double w[pos+neg];		//次の分布D_t+1を求めるためのw
	double W;				//wを正規化するためのもの
	string filename;

	//確率分布の初期化
	for(i=0;i<sample;i++){
		D[i] = 1.0/(double)sample;
		D0[i] = 1.0/(double)sample;
	}

	//優位度の初期化
		 ganma = 0.0;

	//学習データの入力
	filename = "learning_data.txt";
	ifstream fi(filename);
	cout << "positive_sample : " << endl;
	for(i=0;i<pos;i++){
		fi >> positive[i].x1;
		fi >> positive[i].x2;
		positive[i].y = 1;
		cout << "( " << positive[i].x1 << " , " << positive[i].x2 << " )" << endl;
	}
	cout << endl;
	cout << "negative_sample : " << endl;
	for(i=0;i<neg;i++){
		fi >> negative[i].x1;
		fi >> negative[i].x2;
		negative[i].y = -1;
		cout << "( " << negative[i].x1 << " , " << negative[i].x2 << " )" << endl;
	}
	fi.close();

	//訓練フェーズ ====================================================

	for(t=0;t<T;t++){
		//最大優位度の計算
		max_n = 0;
		max_p = 0;
		max_ganma = 0.0;
		for(i=0;i<pos;i++){
			for(j=0;j<neg;j++){
				ganma = yuido(i,j);
				if(ganma>max_ganma){
					max_p = i;
					max_n = j;
					max_ganma = ganma;
				}
			}
		}

		//統合仮説fにmax_pとmax_nを追加
		max.pos_number = max_p;
		max.neg_number = max_n;
		f.push_back(max);

		//重みαの計算
		beta = sqrt((1.0-2.0*max_ganma)/(1.0+2.0*max_ganma));
		alpha.push_back(2.0*log((1.0/beta)));

		//次ステップの確率分布D_t+1を計算
		for(i=0;i<sample;i++){
			if(i<pos){
				temp = pow(beta,(positive[i].y*hypothesis(max_p,max_n,i)));
				w[i] = D[i]*temp;
			}
			else{
				temp = pow(beta,(negative[i-pos].y*hypothesis(max_p,max_n,i)));
				w[i] = D[i]*temp;
			}
		}
		W = 0.0;
		for(i=0;i<sample;i++){
			W += w[i];
		}
		for(i=0;i<sample;i++){
			D[i] = w[i]/W;
		}

		
		//デバッグ用
		cout << "最大優位度" <<max_ganma << endl;
		cout << "max_p " << max_p << endl;
		cout << "max_n " << max_n << endl;
//		cout << "f.max_p " <<f[0].pos_number << endl;
//		cout << "f.max_n " <<f[0].neg_number << endl;
//		cout << "beta " << beta << endl;
//		cout << "alpha " << alpha[0] <<endl;
//		cout << "w " << w[12] << endl;
//		cout << "W " << W << endl;
//		cout << "D[0]" << D[10] << endl;
		
	}
	//========================================================


	//テストフェーズ==========================================
	double temp_ans;
	double ans;
	int flag = 1;
	filename = "test.txt";
	ifstream fi2(filename);

	while(1){
		//テストデータの入力
		fi2 >> test.x1;
		fi2 >> test.x2;

		//統合仮説による判別
		temp_ans = 0.0;
		for(i=0;i<f.size();i++){
			temp_ans += alpha[i]*hypothesis_test(f[i].pos_number,f[i].neg_number);
		}
		ans = sign(temp_ans);

		//結果出力
		cout << "test_data : ( " << test.x1 << " , " << test.x2 << " )" << endl; 
		cout << "label is " << ans << endl;
		cout << endl;

		if(fi2.eof())
				break;

	}
	fi2.close();
}

// 仮説の計算
int hypothesis(int x_pos,int x_neg,int x){
	double d1;
	double d2;
	double h;

	double a,b,c,d,e,f;
	double temp;

	if(x<pos){
		a = positive[x].x1;
		b = positive[x].x2;
		c = negative[x_neg].x1;
		d = negative[x_neg].x2;
		e = positive[x_pos].x1;
		f = positive[x_pos].x2;
/*
		cout << "a=" << a << endl;
		cout << "b=" << b << endl;
		cout << "c=" << c << endl;
		cout << "d=" << d << endl;
		cout << "e=" << e << endl;
		cout << "f=" << f << endl;
*/
		temp = (a-c)*(a-c)+(b-d)*(b-d);
		d1 = sqrt(temp);
//		cout << "d1=" << d1 << endl;
		temp = (a-e)*(a-e)+(b-f)*(b-f);
		d2 = sqrt(temp);
//		cout << "d2=" << d2 << endl;
/*
		d1 = hypot((negative[x_neg].x1-positive[x].x1),(negative[x_neg].x2-positive[x].x2)); 
		d2 = hypot((positive[x_pos].x1-positive[x].x1),(positive[x_pos].x2-positive[x].x2)); 
		cout << d1 << endl;
		cout << d2 << endl;
*/
		h = d1-d2;
		if(h>=0)
			return 1;
		else
			return -1;
	}
	else{
		//cout << "マイナス" << endl;
		//cout << "x=" << x-pos << endl;
		a = negative[x-pos].x1;
		b = negative[x-pos].x2;
		c = negative[x_neg].x1;
		d = negative[x_neg].x2;
		e = positive[x_pos].x1;
		f = positive[x_pos].x2;
/*
		cout << "a=" << a << endl;
		cout << "b=" << b << endl;
		cout << "c=" << c << endl;
		cout << "d=" << d << endl;
		cout << "e=" << e << endl;
		cout << "f=" << f << endl;
*/
		temp = (a-c)*(a-c)+(b-d)*(b-d);
		d1 = sqrt(temp);
//		cout << "d1=" << d1 << endl;
		temp = (a-e)*(a-e)+(b-f)*(b-f);
		d2 = sqrt(temp);
//		cout << "d2=" << d2 << endl;
/*
		d1 = hypot((negative[x_neg].x1-negative[x-pos].x1),(negative[x_neg].x2-negative[x-pos].x2)); 
		d2 = hypot((positive[x_pos].x1-negative[x-pos].x1),(negative[x_pos].x2-negative[x-pos].x2)); 
		cout << d1 << endl;
		cout << d2 << endl;
*/
		h = d1-d2;
		if(h>=0)
			return 1;
		else
			return -1;
	}

}

//優位度の計算
double yuido(int x_pos,int x_neg){
	int i;
	double temp=0.0;
	double ganma;

	for(i=0;i<pos;i++){
//		cout << (double)hypothesis(x_pos,x_neg,i)*(double)positive[i].y << endl;
		temp += D[i]*(double)hypothesis(x_pos,x_neg,i)*(double)positive[i].y;
//		cout << "temp" << i <<" " << temp << endl;
//		cout << endl;
	}
	for(i=0;i<neg;i++){
//		cout << (double)hypothesis(x_pos,x_neg,i+pos)*(double)negative[i].y << endl;
		temp += D[i+pos]*(double)hypothesis(x_pos,x_neg,i+pos)*(double)negative[i].y;
//		cout << "temp" << i <<" " << temp << endl;
//		cout << endl;
	}
	ganma = temp /2.0;
	return ganma;
}

// テスト時の仮説の計算
int hypothesis_test(int x_pos,int x_neg){
	double d1;
	double d2;
	double h;

	double a,b,c,d,e,f;
	double temp;

		a = test.x1;
		b = test.x2;
		c = negative[x_neg].x1;
		d = negative[x_neg].x2;
		e = positive[x_pos].x1;
		f = positive[x_pos].x2;
/*
		cout << "a=" << a << endl;
		cout << "b=" << b << endl;
		cout << "c=" << c << endl;
		cout << "d=" << d << endl;
		cout << "e=" << e << endl;
		cout << "f=" << f << endl;
*/
		temp = (a-c)*(a-c)+(b-d)*(b-d);
		d1 = sqrt(temp);
//		cout << "d1=" << d1 << endl;
		temp = (a-e)*(a-e)+(b-f)*(b-f);
		d2 = sqrt(temp);
//		cout << "d2=" << d2 << endl;
/*
		d1 = hypot((negative[x_neg].x1-positive[x].x1),(negative[x_neg].x2-positive[x].x2)); 
		d2 = hypot((positive[x_pos].x1-positive[x].x1),(positive[x_pos].x2-positive[x].x2)); 
		cout << d1 << endl;
		cout << d2 << endl;
*/
		h = d1-d2;
		if(h>=0)
			return 1;
		else
			return -1;
}

int sign(double x){
	if(x >= 0)
		return 1;

	else 
		return -1;
}