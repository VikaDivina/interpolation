#include <iostream>
#include <iomanip>
#include <ctime>
#include <cmath>
using namespace std;
double divided_difference_for_Newton(double**, int);
double polinom_Newtona (double**, int, double);
void formula_polinom_Newtona (double**, int);
double polinom_general(double**, int, double);  
void formula_polinom_general(double**, int);            
void build_graph(double**, int, double (*polinom)(double**, int, double));
int main()
{
    double** points;
    int n;
    cout << "Введите количество точек: ";
    cin >> n;
    if (n>1)
    {
        points = new double* [n];
        for (int i=0; i<n; i++) points[i] = new double[2];
    }
    else 
    {
        cout << "Некорректный ввод количества точек" << endl;
        return 0;
    }

    srand(time(0));
    cout << "Ваши точки: " << endl;
    for(int i=0; i<n; i++)
    {
        points[i][0] = -100 + rand()%200;
        points[i][1] = -100 + rand()%200;
        cout << i+1 << " точка: " << points[i][0] << " " << points[i][1] << endl;
    }


    // проверка чтобы у каждого х был только 1 у (надо проверить, не повторяются ли х)
    for(int i = 0; i < n; i++) 
    for(int j = i + 1; j < n; j++) 
    if(points[i][0] == points[j][0])
    {
        cout << "Ошибка ввода данных" << endl;
        for(int i=0; i<n; i++) delete[]points[i];
        delete[]points;
        points = nullptr;
        return 0;
    }

    int k;
    cout << "Выберите способ аппроксимации: 1 - интерполяция многочлена Ньютона; 2 - интерполяция общего вида(через расчет коэффициентов через многочлен Лагранжа) : "; 
    cin >> k;
    switch (k)
    {
        case 1:
        {
            cout << "Формула выглядит следующим образом: ";
        formula_polinom_Newtona(points, n);
            //тестовая проверка работы полинома ньютона
            for(int i=0; i<n; i++) cout << "для точки " << points[i][0] << " : " << polinom_Newtona(points, n, points[i][0]) << endl;
            build_graph(points, n, polinom_Newtona);
            break;
        }
        case 2:
        {
            cout << "Формула выглядит следующим образом: ";
            formula_polinom_general(points, n);  
            //тестовая проверка работы полинома общего вида
            for (int k=0; k<n; k++) cout << "для точки " << points[k][0] << " : " << polinom_general(points, n, points[k][0]) << endl;  
            build_graph(points, n, polinom_general);
            break;
        } 
        
        default:
            cout << "Такого варианта нет";
            break;
    }
    
    
    for(int i=0; i<n; i++) delete[]points[i];
    delete[]points;
    points = nullptr;
    return 0;
}

// П О Л И Н О М   Н Ь Ю Т О Н А

double divided_difference_for_Newton(double** points, int number_of_x)
{
    double sum = 0, multiplication;
    for(int i=0; i<number_of_x; i++)
    {
        multiplication = 1;
        for(int j=0; j<number_of_x; j++)
        {
            if(i != j) multiplication *= (points[i][0] - points[j][0]);
        }
        sum += points[i][1]/multiplication;
    }
    return sum;
}

double polinom_Newtona (double** points, int n, double x) // points - массив точек (х, у). n - количество точек. x - любая х, принадлежащая функции, для которой считается у
{
    double result = points[0][1], mul;
    for(int i=1; i<n; i++)
    {
        mul=1;
        for(int j=0; j<i; j++)
        {
            mul *= (x-points[j][0]);
        }
        result += divided_difference_for_Newton(points, i+1)*mul;
    }
    return result;
}

void formula_polinom_Newtona (double** points, int n) // points - массив точек (х, у). n - количество точек. 
{
    double result = points[0][1], mul;
    cout << "y = " << points[0][1];
    double difference=0;
    for(int i=1; i<n; i++)
    {
        mul=1;
        difference = divided_difference_for_Newton(points, i+1);
        if(difference>0) cout << " + " << setprecision(2) << difference;
        else if(difference<0) cout << " - " << setprecision(2) << -1*difference;
        else continue;
        for(int j=0; j<i; j++)
        {
            if(points[j][0]>0) cout << "(x-" << points[j][0] << ")";
            else cout << "(x+" << -1*points[j][0] << ")";
            
        }
    }
    cout << endl;
}

// И Н Т Е Р П О Л Я Ц И Я   О Б Щ Е Г О   В И Д А   

// a[k][j] - коэффициенты базисных многочленов Лагранжа
// c[j] - коэффициенты результирующего многочлена
void coefficients(double** points, int n, double* c) 
{
    // матрица коэффициентов a[k][j]
    double** a = new double*[n];
    for (int k = 0; k < n; k++) 
    {
        a[k] = new double[n];
    }
    
    // инициализация
    for (int k = 0; k < n; k++) 
    {
        a[k][0] = 1.0;
        for (int j = 1; j < n; j++) 
        {
            a[k][j] = 0.0;
        }
    }
    
    for (int k = 0; k < n; k++) 
    {
        for (int i = 0; i < n; i++) 
        {
            if (i == k) continue;
            
            double d = points[k][0] - points[i][0];
            
            double* old_a = new double[n];
            for (int j = 0; j < n; j++) 
            {
                old_a[j] = a[k][j];
            }
            
            a[k][0] = -old_a[0] * points[i][0] / d;
            
            for (int j = 1; j < n; j++) 
            {
                a[k][j] = (old_a[j-1] - old_a[j] * points[i][0]) / d;
            }
            
            delete[] old_a;
        }
    }
    
    // коэффициенты результирующего многочлена
    for (int i = 0; i < n; i++) 
    {
        c[i] = 0.0;
        for (int k = 0; k < n; k++) 
        {
            c[i] += points[k][1] * a[k][i];
        }
    }
    
    for (int k = 0; k < n; k++) 
    {
        delete[] a[k];
    }
    delete[] a;
}

double polinom_general(double** points, int n, double x) 
{
    double* c = new double[n];
    coefficients(points, n, c);
    double result = 0.0;
    for (int i = 0; i < n; i++) 
    {
        result += c[i] * pow(x, i);
    }
    
    delete[] c;
    return result;
}

void formula_polinom_general(double** points, int n) 
{
    double* c = new double[n];
    coefficients(points, n, c);
    
    cout << "y = ";
    bool first = true;
    for (int i = 0; i < n; i++) 
    {
        if (c[i] == 0) continue;  
        
        if (first) 
        {
            if (i == 0) 
            {
                cout << setprecision(2) << c[i];
            } else 
            {
                cout << setprecision(2) << c[i];
                if (i == 1) cout << "*x";
                else cout << "*x^" << i;
            }
            first = false;
        } else 
        {
            if (c[i] > 0) cout << " + ";
            else cout << " - ";
            
            double abs_c = (c[i] > 0) ? c[i] : -c[i];
            if (i == 0) {
                cout << setprecision(2) << abs_c;
            } else {
                cout << setprecision(2) << abs_c;
                if (i == 1) cout << "*x";
                else cout << "*x^" << i;
            }
        }
    }
    cout << endl;
    delete[] c;
}


// Ф У Н К Ц И Я   Д Л Я   П О С Т Р О Е Н И Я   Г Р А Ф И К А 

void build_graph(double** points, int n, double (*polinom)(double**, int, double))
{
    
    double min_x = points[0][0];
    double max_x = points[0][0];
    double min_y = points[0][1];
    double max_y = points[0][1];
    
    for(int i = 1; i < n; i++)
    {
        if(points[i][0] < min_x) min_x = points[i][0];
        if(points[i][0] > max_x) max_x = points[i][0];
        if(points[i][1] < min_y) min_y = points[i][1];
        if(points[i][1] > max_y) max_y = points[i][1];
    }

    int hight=40, width=80;
    double n_x=1; // во сколько раз надо ужать 
    if(fabs(max_x)>width || fabs(min_x)>width) 
    {
        double max_x_mod = (fabs(max_x)>fabs(min_x)) ? fabs(max_x) : fabs(min_x);
        n_x = max_x_mod/width;
    }
    

    double n_y=1; // во сколько раз надо ужать 
    double step = fabs(max_x - min_x)/300.;
    double now_x = min_x, maximum_y = max_y, minimum_y = min_y, now_y;
    while(now_x<=max_x)
    {
        now_y = polinom(points, n, now_x);
        if(now_y > maximum_y) maximum_y = now_y;
        if(now_y < minimum_y) minimum_y = now_y;
        now_x += step;
    }
    n_y = (fabs(maximum_y)>fabs(minimum_y)) ? fabs(maximum_y)/hight : fabs(minimum_y)/hight;

    double num;
    num = (n_y>n_x) ? n_y : n_x;
    num *= 1.2;
    
    cout << endl << "График выглядит следующим образом: " << endl;
    cout << "Масштаб " << setprecision(5) << num  << ":1 "<< endl;
    
    for(int i=-width; i<0; i++) cout << " ";
    cout << "y" << endl << endl;
    for(int i=-width; i<0; i++) cout << " ";
    cout << "^" << endl;
    double x, m;
    int k;
    if(num>5) m=num/sqrt(num);
    else m=num;
    for(int j=hight; j>-hight; j--)
    {    
        if(j==0)
        {
            for(int i=-width; i<=width; i++)
            {
                x = polinom(points, n, i*m);
                if(x >= j*num-num && x <= j*num+num) cout << ".";
                else if(i==0) cout << "+";
                else cout << "-";
            }
            cout << "> x" << endl;
            continue;
        }
        for(int i=-width; i<=width; i++)
        {
            x = polinom(points, n, i*m);
            if(i==0) 
            {
                cout << "-";
                continue;
            } 
            if(x >= j*num-num && x <= j*num+num) cout << ".";
            else cout << " ";
        }
        cout << endl;
    }
}
