#include <curses.h>
#include <stdlib.h>

//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor
{
	NONE,
	RED,
	BLUE,
	GREEN,
	YELLOW
};

class PuyoArray
{
    public:
        PuyoArray() : data(NULL), data_line(0), data_column(0)
        {
        }
        ~PuyoArray()
        {
            Release();
        }

    private:
        //盤面状態
        enum puyocolor *data;
        //盤面の行数、列数
        unsigned int data_line;
        unsigned int data_column;
        //メモリ開放
        void Release()
        {
            if (data == NULL)
            {
                return;
            }

            delete[] data;
            data = NULL;
        }

    public:
        //盤面サイズ変更
        void ChangeSize(unsigned int line, unsigned int column)
        {
            Release();

            //新しいサイズでメモリ確保
            data = new puyocolor[line * column];

            data_line = line;
            data_column = column;
        }

        //盤面の行数を返す
        unsigned int GetLine()
        {
            return data_line;
        }

        //盤面の列数を返す
        unsigned int GetColumn()
        {
            return data_column;
        }

        //盤面の指定された位置の値を返す
        puyocolor GetValue(unsigned int y, unsigned int x)
        {
            if (y >= GetLine() || x >= GetColumn())
            {
                //引数の値が正しくない
                return NONE;
            }

            return data[y * GetColumn() + x];
        }

        //盤面の指定された位置に値を書き込む
        void SetValue(unsigned int y, unsigned int x, puyocolor value)
        {
            if (y >= GetLine() || x >= GetColumn())
            {
                //引数の値が正しくない
                return;
            }

            data[y * GetColumn() + x] = value;
        }
};

//落下中ぷよの管理
class PuyoArrayActive : public PuyoArray
{
    public:
        int puyorotate;

    public:
        PuyoArrayActive(){};
};

//着地済みぷよの管理
class PuyoArrayStack : public PuyoArray
{
    public:
};

class PuyoControl
{
    public:
        int puyopoint = 0;

    public:
        //盤面に新しいぷよ生成
        void GeneratePuyo(PuyoArrayActive &puyoactive)
        {
            puyocolor newpuyo1;
            newpuyo1 = RED;

            puyocolor newpuyo2;
            newpuyo2 = BLUE;

            puyocolor newpuyo3;
            newpuyo3 = GREEN;

            puyocolor newpuyo4;
            newpuyo4 = YELLOW;

            int color_define1 = rand() % 5;
            int color_define2 = rand() % 5;

            puyoactive.puyorotate = 0;

            if (color_define1 == 1)
            {
                puyoactive.SetValue(0, 5, newpuyo1);
            }
            else if (color_define1 == 2)
            {
                puyoactive.SetValue(0, 5, newpuyo2);
            }
            else if (color_define1 == 3)
            {
                puyoactive.SetValue(0, 5, newpuyo3);
            }
            else
            {
                puyoactive.SetValue(0, 5, newpuyo4);
            }

            if (color_define2 == 1)
            {
                puyoactive.SetValue(0, 6, newpuyo1);
            }
            else if (color_define2 == 2)
            {
                puyoactive.SetValue(0, 6, newpuyo2);
            }
            else if (color_define2 == 3)
            {
                puyoactive.SetValue(0, 6, newpuyo3);
            }
            else
            {
                puyoactive.SetValue(0, 6, newpuyo4);
            }
        }

        //ぷよの着地判定．着地判定があるとtrueを返す
        bool LandingPuyo(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack)
        {
            bool landed = false;

            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {
                    if (puyoactive.GetValue(y, x) != NONE && puyostack.GetValue(y, x) == NONE && (y == puyoactive.GetLine() - 1 || puyostack.GetValue(y + 1, x) != NONE))
                    {

                        landed = true;
                        //puyoStackに保存
                        puyostack.SetValue(y, x, puyoactive.GetValue(y, x));
                        //puyoActiveでは削除
                        puyoactive.SetValue(y, x, NONE);

                        if (puyoactive.GetValue(y - 1, x) != NONE)
                        {
                            //puyoStackに保存
                            puyostack.SetValue(y - 1, x, puyoactive.GetValue(y - 1, x));
                            //puyoActiveでは削除
                            puyoactive.SetValue(y - 1, x, NONE);
                        }

                        //ぷよが一番上に来たときの処理　　　
                        //if(y==0 && puyostack.GetValue(y+1,x) != NONE)
                        //{
                        //puyoStackに保存
                        //	puyostack.SetValue(y, x, puyoactive.GetValue(y, x));
                        //puyoActiveでは削除
                        //	puyoactive.SetValue(y, x, NONE);
                        //}
                    }
                }
            }
            return landed;
        }

        //空中にぷよがあるかを判定
        bool Remainpuyo(PuyoArrayStack &puyostack)
        {
            bool remainpuyo = false;

            for (int y = 0; y < puyostack.GetLine(); y++)
            {
                for (int x = 0; x < puyostack.GetColumn(); x++)
                {
                    if (puyostack.GetValue(y - 1, x) == NONE)
                    {
                        remainpuyo = true;
                    }
                }
            }
            return remainpuyo;
        }

        //puyoactiveがあったらcountに+1
        int PuyoCount(PuyoArrayActive &puyoactive)
        {
            int count = 0;
            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {
                    if (puyoactive.GetValue(y, x) != NONE)
                    {
                        count++;
                    }
                }
            }
                return count;
        }

        //ぷよ消滅処理を全座標で行う
        //消滅したぷよの数を返す
        int VanishPuyo(PuyoArrayStack &puyostack)
        {
            int vanishednumber = 0;
            for (int y = 0; y < puyostack.GetLine(); y++)
            {
                for (int x = 0; x < puyostack.GetColumn(); x++)
                {
                    vanishednumber += VanishPuyo(puyostack, y, x);
                }
            }

            return vanishednumber;
        }

        //ぷよ消滅処理を座標(x,y)で行う
        //消滅したぷよの数を返す
        int VanishPuyo(PuyoArrayStack &puyostack, unsigned int y, unsigned int x)
        {
            //判定個所にぷよがなければ処理終了
            if (puyostack.GetValue(y, x) == NONE)
            {
                return 0;
            }

            //判定状態を表す列挙型
            //NOCHECK判定未実施，CHECKINGが判定対象，CHECKEDが判定済み
            enum checkstate
            {
                NOCHECK,
                CHECKING,
                CHECKED
            };

            //判定結果格納用の配列
            enum checkstate *field_array_check;
            field_array_check = new enum checkstate[puyostack.GetLine() * puyostack.GetColumn()];

            //配列初期化
            for (int i = 0; i < puyostack.GetLine() * puyostack.GetColumn(); i++)
            {
                field_array_check[i] = NOCHECK;
            }

            //座標(x,y)を判定対象にする
            field_array_check[y * puyostack.GetColumn() + x] = CHECKING;

            //判定対象が1つもなくなるまで，判定対象の上下左右に同じ色のぷよがあるか確認し，あれば新たな判定対象にする
            bool checkagain = true;
            while (checkagain)
            {
                checkagain = false;

                for (int y = 0; y < puyostack.GetLine(); y++)
                {
                    for (int x = 0; x < puyostack.GetColumn(); x++)
                    {
                        //(x,y)に判定対象がある場合
                        if (field_array_check[y * puyostack.GetColumn() + x] == CHECKING)
                        {
                            //(x+1,y)の判定
                            if (x < puyostack.GetColumn() - 1)
                            {
                                //(x+1,y)と(x,y)のぷよの色が同じで，(x+1,y)のぷよが判定未実施か確認
                                if (puyostack.GetValue(y, x + 1) == puyostack.GetValue(y, x) && field_array_check[y * puyostack.GetColumn() + (x + 1)] == NOCHECK)
                                {
                                    //(x+1,y)を判定対象にする
                                    field_array_check[y * puyostack.GetColumn() + (x + 1)] = CHECKING;
                                    checkagain = true;
                                }
                            }

                            //(x-1,y)の判定
                            if (x > 0)
                            {
                                if (puyostack.GetValue(y, x - 1) == puyostack.GetValue(y, x) && field_array_check[y * puyostack.GetColumn() + (x - 1)] == NOCHECK)
                                {
                                    field_array_check[y * puyostack.GetColumn() + (x - 1)] = CHECKING;
                                    checkagain = true;
                                }
                            }

                            //(x,y+1)の判定
                            if (y < puyostack.GetLine() - 1)
                            {
                                if (puyostack.GetValue(y + 1, x) == puyostack.GetValue(y, x) && field_array_check[(y + 1) * puyostack.GetColumn() + x] == NOCHECK)
                                {
                                    field_array_check[(y + 1) * puyostack.GetColumn() + x] = CHECKING;
                                    checkagain = true;
                                }
                            }

                            //(x,y-1)の判定
                            if (y > 0)
                            {
                                if (puyostack.GetValue(y - 1, x) == puyostack.GetValue(y, x) && field_array_check[(y - 1) * puyostack.GetColumn() + x] == NOCHECK)
                                {
                                    field_array_check[(y - 1) * puyostack.GetColumn() + x] = CHECKING;
                                    checkagain = true;
                                }
                            }

                            //(x,y)を判定済みにする
                            field_array_check[y * puyostack.GetColumn() + x] = CHECKED;
                        }
                    }
                }
            }

            //判定済みの数をカウント

            int puyocount = 0;
            for (int i = 0; i < puyostack.GetLine() * puyostack.GetColumn(); i++)
            {
                if (field_array_check[i] == CHECKED)
                {
                    puyocount++;
                }
            }

            //4個以上あれば，判定済み座標のぷよを消す
            int vanishednumber = 0;
            if (4 <= puyocount)
            {
                for (int y = 0; y < puyostack.GetLine(); y++)
                {
                    for (int x = 0; x < puyostack.GetColumn(); x++)
                    {
                        if (field_array_check[y * puyostack.GetColumn() + x] == CHECKED)
                        {
                            puyostack.SetValue(y, x, NONE);

                            vanishednumber++;
                            puyopoint++;
                        }
                    }
                }
            }

            //メモリ解放
            delete[] field_array_check;

            return vanishednumber;
            return puyopoint;
        }

        //得点表示のための変数
        int PuyoPoint()
        {
            return puyopoint;
        }

        //回転
        //PuyoArrayActiveクラスのprivateメンバ変数として int puyorotate を宣言し，これに回転状態を記憶させている．
        //puyorotateにはコンストラクタ及びGeneratePuyo関数で値0を代入する必要あり．
        void Rotate(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack)
        {
            //フィールドをラスタ順に探索（最も上の行を左から右方向へチェックして，次に一つ下の行を左から右方向へチェックして，次にその下の行・・と繰り返す）し，先に発見される方をpuyo1, 次に発見される方をpuyo2に格納
            puyocolor puyo1, puyo2;
            int puyo1_x = 0;
            int puyo1_y = 0;
            int puyo2_x = 0;
            int puyo2_y = 0;

            bool findingpuyo1 = true;
            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {
                    if (puyoactive.GetValue(y, x) != NONE)
                    {
                        if (findingpuyo1)
                        {
                            puyo1 = puyoactive.GetValue(y, x);
                            puyo1_x = x;
                            puyo1_y = y;
                            findingpuyo1 = false;
                        }
                        else
                        {
                            puyo2 = puyoactive.GetValue(y, x);
                            puyo2_x = x;
                            puyo2_y = y;
                        }
                    }
                }
            }

            //回転前のぷよを消す
            puyoactive.SetValue(puyo1_y, puyo1_x, NONE);
            puyoactive.SetValue(puyo2_y, puyo2_x, NONE);

            //操作中ぷよの回転
            switch (puyoactive.puyorotate)
            {
            case 0:
                //回転パターン
                //RB -> R
                //      B
                //Rがpuyo1, Bがpuyo2
                //もし回転した結果field_arrayの範囲外に出るなら回転しない
                if (puyo2_x <= 0 || puyo2_y >= puyoactive.GetLine() - 1)
                {
                    puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
                    puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
                    break;
                }

                //回転後の位置にぷよを置く
                puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
                puyoactive.SetValue(puyo2_y + 1, puyo2_x - 1, puyo2);
                //次の回転パターンの設定
                puyoactive.puyorotate = 1;
                break;

            case 1:
                //回転パターン
                //R -> BR
                //B
                //Rがpuyo1, Bがpuyo2
                if (puyo2_x <= 0 || puyo2_y <= 0 || puyostack.GetValue(puyo2_y - 1, puyo1_x - 1) != NONE) //もし回転した結果field_arrayの範囲外に出るなら回転しない
                {
                    puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
                    puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
                    break;
                }

                //回転後の位置にぷよを置く
                puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
                puyoactive.SetValue(puyo2_y - 1, puyo2_x - 1, puyo2);

                //次の回転パターンの設定
                puyoactive.puyorotate = 2;
                break;

            case 2:
                //回転パターン
                //      B
                //BR -> R
                //Bがpuyo1, Rがpuyo2
                if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y <= 0) //もし回転した結果field_arrayの範囲外に出るなら回転しない
                {
                    puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
                    puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
                    break;
                }

                //回転後の位置にぷよを置く
                puyoactive.SetValue(puyo1_y - 1, puyo1_x + 1, puyo1);
                puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

                //次の回転パターンの設定
                puyoactive.puyorotate = 3;
                break;

            case 3:
                //回転パターン
                //B
                //R -> RB
                //Bがpuyo1, Rがpuyo2
                //もし回転した結果field_arrayの範囲外に出るなら回転しない
                if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y >= puyoactive.GetLine() - 1 || puyostack.GetValue(puyo1_y - 1, puyo1_x + 1) != NONE)
                {
                    puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
                    puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
                    break;
                }

                //回転後の位置にぷよを置く
                puyoactive.SetValue(puyo1_y + 1, puyo1_x + 1, puyo1);
                puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

                //次の回転パターンの設定
                puyoactive.puyorotate = 0;
                break;

            default:
                break;
            }
        }

        //空中に残ったぷよの下移動
        void StackMoveDown(PuyoArrayStack &puyostack)
        {
            //一時的格納場所メモリ確保
            puyocolor *puyo_temp = new puyocolor[puyostack.GetLine() * puyostack.GetColumn()];

            for (int i = 0; i < puyostack.GetLine() * puyostack.GetColumn(); i++)
            {
                puyo_temp[i] = NONE;
            }

            //1つ下の位置にpuyostackからpuyo_tempへとコピー
            for (int y = puyostack.GetLine() - 1; y >= 0; y--)
            {
                for (int x = 0; x < puyostack.GetColumn(); x++)
                {
                    if (puyostack.GetValue(y, x) == NONE)
                    {
                        continue;
                    }

                    if (y < puyostack.GetLine() - 1 && puyostack.GetValue(y + 1, x) == NONE)
                    {
                        puyo_temp[(y + 1) * puyostack.GetColumn() + x] = puyostack.GetValue(y, x);
                        //コピー後に元位置のpuyoactiveのデータは消す
                        puyostack.SetValue(y, x, NONE);
                    }
                    else
                    {
                        puyo_temp[y * puyostack.GetColumn() + x] = puyostack.GetValue(y, x);
                    }
                }
            }

            //puyo_tempからpuyostackへコピー
            for (int y = 0; y < puyostack.GetLine(); y++)
            {
                for (int x = 0; x < puyostack.GetColumn(); x++)
                {
                    puyostack.SetValue(y, x, puyo_temp[y * puyostack.GetColumn() + x]);
                }
            }

            //一時的格納場所メモリ解放
            delete[] puyo_temp;
        }

        //左移動
        void MoveLeft(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack)
        {
            //一時的格納場所メモリ確保
            puyocolor *puyo_temp = new puyocolor[puyoactive.GetLine() * puyoactive.GetColumn()];

            for (int i = 0; i < puyoactive.GetLine() * puyoactive.GetColumn(); i++)
            {
                puyo_temp[i] = NONE;
            }

            //1つ左の位置にpuyoactiveからpuyo_tempへとコピー
            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {
                    //左にぷよがあった場合には移動しない
                    if (puyostack.GetValue(y, x - 1) != NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                        //puyoactive.SetValue(y, x , NONE);
                    }
                    else if (puyostack.GetValue(y, x - 2) != NONE && puyoactive.GetValue(y, x - 1) != NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                        //puyoactive.SetValue(y, x, NONE);
                    }

                    if (puyoactive.GetValue(y, x) == NONE)
                    {
                        continue;
                    }

                    if (0 < x && puyoactive.GetValue(y, x - 1) == NONE && puyostack.GetValue(y, x - 1) == NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + (x - 1)] = puyoactive.GetValue(y, x);
                        //コピー後に元位置のpuyoactiveのデータは消す
                        puyoactive.SetValue(y, x, NONE);
                    }
                    else
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                    }
                }
            }

            //puyo_tempからpuyoactiveへコピー
            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {
                    puyoactive.SetValue(y, x, puyo_temp[y * puyoactive.GetColumn() + x]);
                }
            }

            //一時的格納場所メモリ解放
            delete[] puyo_temp;
        }

        //右移動
        void MoveRight(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack)
        {
            //一時的格納場所メモリ確保
            puyocolor *puyo_temp = new puyocolor[puyoactive.GetLine() * puyoactive.GetColumn()];

            for (int i = 0; i < puyoactive.GetLine() * puyoactive.GetColumn(); i++)
            {
                puyo_temp[i] = NONE;
            }

            //1つ右の位置にpuyoactiveからpuyo_tempへとコピー
            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = puyoactive.GetColumn() - 1; x >= 0; x--)
                {

                    //右にぷよがあった場合には移動しない
                    if (puyostack.GetValue(y, x + 1) != NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                        //コピー後に元位置のpuyoactiveのデータは消す
                        //puyoactive.SetValue(y, x, NONE);
                    }
                    else if (puyostack.GetValue(y, x + 2) != NONE && puyoactive.GetValue(y, x + 2) != NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                        //コピー後に元位置のpuyoactiveのデータは消す
                        //puyoactive.SetValue(y, x, NONE);
                    }

                    if (puyoactive.GetValue(y, x) == NONE)
                    {
                        continue;
                    }

                    if (x < puyoactive.GetColumn() - 1 && puyoactive.GetValue(y, x + 1) == NONE && puyostack.GetValue(y, x + 1) == NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + (x + 1)] = puyoactive.GetValue(y, x);
                        //コピー後に元位置のpuyoactiveのデータは消す
                        puyoactive.SetValue(y, x, NONE);
                    }

                    else
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                    }
                }
            }

            //puyo_tempからpuyoactiveへコピー
            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {
                    puyoactive.SetValue(y, x, puyo_temp[y * puyoactive.GetColumn() + x]);
                }
            }
            //一時的格納場所メモリ解放
            delete[] puyo_temp;
        }

        //下移動
        void MoveDown(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack)
        {
            //一時的格納場所メモリ確保
            puyocolor *puyo_temp = new puyocolor[puyoactive.GetLine() * puyoactive.GetColumn()];

            for (int i = 0; i < puyoactive.GetLine() * puyoactive.GetColumn(); i++)
            {
                puyo_temp[i] = NONE;
            }

            for (int y = puyoactive.GetLine() - 1; y >= 0; y--)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {

                    if (puyostack.GetValue(y + 1, x) != NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                        //コピー後に元位置のpuyoactiveのデータは消す
                        //puyoactive.SetValue(y, x, NONE);
                    }

                    else if (puyostack.GetValue(y + 2, x) != NONE && puyoactive.GetValue(y + 1, x) != NONE)
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                    }

                    if (puyoactive.GetValue(y, x) == NONE)
                    {
                        continue;
                    }

                    if (y < puyoactive.GetLine() - 1 && puyoactive.GetValue(y + 1, x) == NONE && puyostack.GetValue(y + 1, x) == NONE)
                    {
                        puyo_temp[(y + 1) * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                        //コピー後に元位置のpuyoactiveのデータは消す
                        puyoactive.SetValue(y, x, NONE);
                    }
                    else
                    {
                        puyo_temp[y * puyoactive.GetColumn() + x] = puyoactive.GetValue(y, x);
                    }
                }
            }

            //puyo_tempからpuyoactiveへコピー
            for (int y = 0; y < puyoactive.GetLine(); y++)
            {
                for (int x = 0; x < puyoactive.GetColumn(); x++)
                {
                    puyoactive.SetValue(y, x, puyo_temp[y * puyoactive.GetColumn() + x]);
                }
            }

            //一時的格納場所メモリ解放
            delete[] puyo_temp;
        }


        //ゲームオーバーの判定、一番上の列まで積んだらゲームオーバー
        bool GameoverJudge(PuyoArrayStack &puyostack)
        {
            bool gameover = false;
            for (int y = puyostack.GetLine() - 1; y >= 0; y--)
                for (int x = 0; x < puyostack.GetColumn(); x++)
                {
                    if (y == 0 && puyostack.GetValue(y, x) != NONE)
                    {
                        gameover = true;
                    }
                }
            return gameover;
        }
};

//スタート画面の表示
void StartDisplay()
{
	clear();
	mvprintw(LINES / 2 - 4, (COLS - 72) / 2, "PPPP     U    U    Y     Y     OOOO      PPPP     U    U    Y     Y    OOOO        ");
	mvprintw(LINES / 2 - 3, (COLS - 72) / 2, "P   P    U    U     Y   Y     O    O     P   P    U    U     Y   Y    O    O       ");
	mvprintw(LINES / 2 - 2, (COLS - 72) / 2, "P   P    U    U      Y Y      O    O     P   P    U    U      Y Y     O    O       ");
	mvprintw(LINES / 2 - 1, (COLS - 72) / 2, "PPPP     U    U       Y       O    O     PPPP     U    U       Y      O    O       ");
	mvprintw(LINES / 2, (COLS - 72) / 2, "P        U    U       Y       O    O     P        U    U       Y      O    O       ");
	mvprintw(LINES / 2 + 1, (COLS - 72) / 2, "P        U    U       Y       O    O     P        U    U       Y      O    O       ");
	mvprintw(LINES / 2 + 2, (COLS - 72) / 2, "P        U    U       Y       O    O     P        U    U       Y      O    O       ");
	mvprintw(LINES / 2 + 3, (COLS - 72) / 2, "P         UUUU        Y        OOOO      P         UUUU        Y       OOOO        ");
	mvprintw(LINES / 2 + 4, (COLS - 72) / 2, "                                                                                   ");
	mvprintw(LINES / 2 + 5, (COLS - 72) / 2, "                                                              ver.1.7.0            ");
	mvprintw(LINES / 2 + 6, (COLS - 72) / 2, "                                                                                   ");
	mvprintw(LINES / 2 + 7, (COLS - 72) / 2, "                              made by Hayato Saito                                 ");
	mvprintw(LINES / 2 + 8, (COLS - 72) / 2, "                                                                                   ");
	mvprintw(LINES / 2 + 9, (COLS - 72) / 2, "                                                                                   ");
	mvprintw(LINES / 2 + 10, (COLS - 72) / 2, "                              [PRESS S TO START]                                   ");
	mvprintw(LINES / 2 + 11, (COLS - 72) / 2, "                              [PRESS Q TO QUIT ]                                   ");
}

//ゲームオーバーの表示
void printGameOver(PuyoControl &control)
{
	/* 端末の幅によって表示方法変更 */
	if (COLS > 150)
	{
		clear();
		mvprintw(LINES / 2 - 4, (COLS - 70) / 2, " GGGG    AAA   M     M  EEEEE        OOOO   V     V  EEEEE  RRRR       ");
		mvprintw(LINES / 2 - 3, (COLS - 70) / 2, "G    G  A   A  MM   MM  E           O    O  V     V  E      R   R      ");
		mvprintw(LINES / 2 - 2, (COLS - 70) / 2, "G       A   A  M M M M  E           O    O  V     V  E      R    R     ");
		mvprintw(LINES / 2 - 1, (COLS - 70) / 2, "G       AAAAA  M  M  M  EEEEE       O    O  V     V  EEEEE  RRRRR      ");
		mvprintw(LINES / 2, (COLS - 70) / 2, "G       A   A  M     M  E           O    O  V     V  E      R  R       ");
		mvprintw(LINES / 2 + 1, (COLS - 70) / 2, "G   GGG A   A  M     M  E           O    O   V   V   E      R   R      ");
		mvprintw(LINES / 2 + 2, (COLS - 70) / 2, "G    G  A   A  M     M  E           O    O    V V    E      R    R     ");
		mvprintw(LINES / 2 + 3, (COLS - 70) / 2, " GGGG   A   A  M     M  EEEEE        OOOO      V     EEEEE  R     R    ");
		mvprintw(LINES / 2 + 4, (COLS - 70) / 2, "                                                                       ");
		mvprintw(LINES / 2 + 5, (COLS - 70) / 2, "                  YOUR SCORE    %d                                     ", control.PuyoPoint()*10);
		mvprintw(LINES / 2 + 6, (COLS - 70) / 2, "                             [PRESS Q]                                 ");
	}
	else
	{
		clear();
		mvprintw(LINES / 2, (COLS - 10) / 2, "GAME OVER");
		mvprintw(LINES / 2 + 1, (COLS - 10) / 2, "YOUR SCORE %d", control.PuyoPoint()*10);
	}
}

//ゲームエンド(ゲームオーバーではない、自分でゲームを終了したときに表示)
void printGameEnd(PuyoControl &control)

{
	/* 端末の幅によって表示方法変更 */
	if (COLS > 150)
	{
		clear();
		mvprintw(LINES / 2 - 4, (COLS - 70) / 2, " GGGG    AAA   M     M  EEEEE         EEEEE  N      N  DDDD    ");
		mvprintw(LINES / 2 - 3, (COLS - 70) / 2, "G    G  A   A  MM   MM  E             E      NN     N  D   D   ");
		mvprintw(LINES / 2 - 2, (COLS - 70) / 2, "G       A   A  M M M M  E             E      N N    N  D    D  ");
		mvprintw(LINES / 2 - 1, (COLS - 70) / 2, "G       AAAAA  M  M  M  EEEEE         EEEEE  N  N   N  D    D  ");
		mvprintw(LINES / 2, (COLS - 70) / 2, "G       A   A  M     M  E             E      N   N  N  D    D  ");
		mvprintw(LINES / 2 + 1, (COLS - 70) / 2, "G   GGG A   A  M     M  E             E      N    N N  D    D  ");
		mvprintw(LINES / 2 + 2, (COLS - 70) / 2, "G    G  A   A  M     M  E             E      N     NN  D   D   ");
		mvprintw(LINES / 2 + 3, (COLS - 70) / 2, " GGGG   A   A  M     M  EEEEE         EEEEE  N      N  DDDD    ");
		mvprintw(LINES / 2 + 4, (COLS - 70) / 2, "                                                                       ");
		mvprintw(LINES / 2 + 5, (COLS - 70) / 2, "                  YOUR SCORE    %d                              ", control.PuyoPoint()*10);
		mvprintw(LINES / 2 + 6, (COLS - 70) / 2, "                             [PRESS Q]                                 ");
	}
	else
	{
		clear();
		mvprintw(LINES / 2, (COLS - 10) / 2, "GAME END");
		mvprintw(LINES / 2 + 1, (COLS - 10) / 2, "YOUR SCORE %d", control.PuyoPoint()*10);
	}
}

//ぷよ画面表示
void Display(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack, PuyoControl &control)
{

	init_pair(0, COLOR_BLACK, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	//落下中ぷよ表示
	for (int y = 0; y < puyoactive.GetLine(); y++)
	{
		for (int x = 0; x < puyoactive.GetColumn(); x++)
		{
			switch (puyoactive.GetValue(y, x))
			{
			case NONE:
				attrset(COLOR_PAIR(0));
				mvaddch(y, x, '.');
				break;
			case RED:
				attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
				attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
				attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
				attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}
		}
	}

	for (int y = 0; y < puyostack.GetLine(); y++)
	{
		for (int x = 0; x < puyostack.GetColumn(); x++)
		{
			switch (puyostack.GetValue(y, x))
			{
			case NONE:
				break;
			case RED:
				attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
				attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
				attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
				attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}
		}
	}

	//情報表示
	int count = 0;
	for (int y = 0; y < puyoactive.GetLine(); y++)
	{
		for (int x = 0; x < puyoactive.GetColumn(); x++)
		{
			if (puyoactive.GetValue(y, x) != NONE)
			{
				count++;
			}
		}
	}

	char msg[256];
	char msg1[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", puyoactive.GetLine(), puyoactive.GetColumn(), count);
	mvaddstr(2, COLS - 35, msg);
	sprintf(msg1, "YOUR SCORE %d", control.PuyoPoint()*10);
	mvaddstr(3, COLS - 35, msg1);
	refresh();
}

//ここから実行される
int main(int argc, char **argv)
{
	//インスタンス生成
	PuyoArray puyo;
	PuyoArrayActive puyoactive;
	PuyoArrayStack puyostack;
	PuyoControl control;
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();

	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);

	//初期化処理
	puyoactive.ChangeSize(LINES / 2, COLS / 2); //フィールドは画面サイズの縦横1/2にする
	puyostack.ChangeSize(LINES / 2, COLS / 2);
	control.GeneratePuyo(puyoactive); //最初のぷよ生成

	int delay = 0;
	int waitCount = 0;

	//画面サイズによって落下速度の調整
	if (COLS > 150)
	{
		waitCount = 3000;
	}
	else
	{
		waitCount = 12000;
	}

	int puyostate = 0;
START:
	StartDisplay();
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();
		if (ch == 'S' || ch == 's')
		{
			clear();
			refresh();
			goto PUYODISPLAY;
		}

		else if (ch == 'Q' || ch == 'q')
		{
			goto END;
		}
	}
	endwin();

PUYODISPLAY:
	//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q' || ch == 'q')
		{
			goto END;
		}

		if (control.GameoverJudge(puyostack))
		{
			goto GAMEOVER;
		}

		//入力キーごとの処理
		switch (ch)
		{
		case KEY_LEFT:
			control.MoveLeft(puyoactive, puyostack);
			break;
		case KEY_RIGHT:
			control.MoveRight(puyoactive, puyostack);
			break;
		case KEY_UP:
			break;
		case KEY_DOWN:
			control.MoveDown(puyoactive, puyostack);
			break;
		case 'z':
			control.Rotate(puyoactive, puyostack);
			break;
		default:
			break;
		}

		//処理速度調整のためのif文
		if (delay % waitCount == 0)
		{
			//ぷよ下に移動
			control.MoveDown(puyoactive, puyostack);

			//ぷよ着地判定
			if (control.LandingPuyo(puyoactive, puyostack) && control.PuyoCount(puyoactive) == 0)
			{
				//着地していたら新しいぷよ生成
				control.GeneratePuyo(puyoactive);
			}

			//ぷよの消去
			control.VanishPuyo(puyostack);

			//空中に残ったぷよの落下
			if (control.Remainpuyo(puyostack))
			{
				control.StackMoveDown(puyostack);
			}
		}
		delay++;

		//表示
		Display(puyoactive, puyostack, control);
	}

GAMEOVER:
	printGameOver(control);
	while (1)
	{
		int ch;
		ch = getch();
		if (ch == 'Q' || ch == 'q')
		{
			clear();
			refresh();
			break;
		}
	}
	endwin();

	return 0;

END:
	printGameEnd(control);
	while (1)
	{
		int ch;
		ch = getch();
		if (ch == 'Q' || ch == 'q')
		{
			clear();
			refresh();
			break;
		}
	}
	endwin();

	return 0;
}
