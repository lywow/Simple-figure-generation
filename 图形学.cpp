#define GLUT_DISABLE_ATEXIT_HACK   

#include <iostream>
#include "triangle.h"
#include <vector>
#include <gl/glut.h>
#include "tinystr.h"
#include "tinyxml.h"
#include <ctime>
#include <windows.h>

using namespace std;

int jishu=0;
vector<Vertex> vvv;
vector<TriangularFace> ttff;
vector<TriangularMesh> ttmm;
vector<Edgeline> edli;
int x = 1;

//输入字符串返回double（包括科学计数）
double returndouble(string str)
{
	double k;
	int e = str.find("e", 0);
	if (e != string::npos)
	{
		int x = atoi(str.substr(e + 2).c_str());
		k = atof(const_cast<const char*>(str.substr(0, e).c_str()));
		while (x--)
		{
			k = k * 0.1;
		}
		//cout <<k<< endl;
	}
	else
	{
		k = atof(const_cast<const char*>(str.c_str()));
	}
	return k;
}

//保存点到vvv顶点表
void savedata1(string str)
{
	int start=0, end=0;
	start = str.find(" ", 0);
	end = str.find(" ", start+1);
	string temp;
	Vertex ve;
	temp = str.substr(0, start);
	ve.x=atof(const_cast<const char*>(temp.c_str()));
	temp = str.substr(start+1, end - start);
	ve.y = atof(const_cast<const char*>(temp.c_str()));
	temp = str.substr(end + 1);
	ve.z= atof(const_cast<const char*>(temp.c_str()));
	ve.index = jishu;
	jishu++;
	vvv.push_back(ve);
}

//保存一个面的三角面片数据到ttff表
void savedata2(string* str)
{
	int start = 0, end = 0;
	TriangularFace tf;
	string temp;

	start = str[0].find(" ", 0);
	end = str[0].find(" ", start + 1);
	temp = str[0].substr(0, start);
	tf.vIndex1 = atoi(temp.c_str());
	temp = str[0].substr(start + 1, end - start);
	tf.vIndex2 = atoi(temp.c_str());
	temp = str[0].substr(end + 1);
	tf.vIndex3 = atoi(temp.c_str());

	start = str[1].find(" ", 0);
	end = str[1].find(" ", start + 1);
	temp = str[1].substr(0, start);
	tf.fn1 = returndouble(temp);
	temp = str[1].substr(start + 1, end - start);
	tf.fn2 = returndouble(temp);
	temp = str[1].substr(end + 1);
	tf.fn3 = returndouble(temp);

	Vertex ve[3];
	for (int i = 0; i < 3; i++)
	{
		start = str[i+2].find(" ", 0);
		end = str[i + 2].find(" ", start + 1);
		temp = str[i + 2].substr(0, start);
		ve[i].x = returndouble(temp);
		temp = str[i + 2].substr(start + 1, end - start);
		ve[i].y = returndouble(temp);
		temp = str[i + 2].substr(end + 1);
		ve[i].z = returndouble(temp);
	}
	tf.vn1 = ve[0];
	tf.vn2 = ve[1];
	tf.vn3 = ve[2];
	ttff.push_back(tf);
}

//读取xml文件，调用savadata1()和savedata2()
void readxml()
{
	cout << "正在调用readxml函数"<< endl;
	string temp;
	string sss[5];
	int i = 0;

	TiXmlDocument* Document = new TiXmlDocument();
	if (!Document->LoadFile("data.xml"))
	{
		cout << "无法加载xml文件！" << endl;
		cin.get();
		return ;
	}
	TiXmlElement* RootElement = Document->RootElement();		//根目录
	cout << "成功加载xml文件" << endl;
	TiXmlElement* NextElement = RootElement->FirstChildElement();		//根目录下的第一个节点层
	TiXmlElement* BoxElement;
	while (NextElement != NULL)		//判断有没有读完
	{
		if (NextElement->ValueTStr() == "shell")//读到shell节点
		{
			TiXmlElement* fac = NextElement->FirstChildElement();//fac为shell下一层节点首节点
			BoxElement = fac->FirstChildElement();//BoxElement为fac下一层子节点
			while(BoxElement!=NULL)		//判断节点有无有无读取完毕
			{
				temp = BoxElement->Attribute("p");
				//cout << "p:" << temp << endl;
				savedata1(temp);//保存顶点数据

				BoxElement = BoxElement->NextSiblingElement();//下一节点
			}
			fac = fac->NextSiblingElement();
			while(fac!=NULL)  //以下读取面片数据
			{
				BoxElement = fac->FirstChildElement();
				while (BoxElement!=NULL)
				{
					sss[0] = BoxElement->Attribute("v");//获取三角面片顶点索引
					//cout << " v:" << temp << endl;

					sss[1] = BoxElement->Attribute("fn");//获取三角面片向量
					//cout << "fb:" << temp << endl;

					TiXmlElement* boxx = BoxElement->FirstChildElement();
					i = 2;
					while (boxx != NULL)
					{
						sss[i] = boxx->Attribute("d");//获取顶点向量
						//cout << "--d:" <<temp<< endl;
						i++;
						boxx = boxx->NextSiblingElement();
					}
					savedata2(sss);
					BoxElement= BoxElement->NextSiblingElement();
				}
				TriangularMesh tm;
				tm.triangles= ttff;
				ttmm.push_back(tm);
				ttff.clear();
				fac = fac->NextSiblingElement();
			}
		}
		NextElement = NextElement->NextSiblingElement();
	}
}

//选出边界边保存
void savedate3(const TriangularFace& tf, int v1 ,int v2)
{
	Edgeline el;
	bool boo = true;
	
	for (vector<Edgeline>::iterator ive = edli.begin(); ive!=edli.end();ive++)//遍历当前边界边池
	{
		if (ive->vindex1==v1&&ive->vindex2==v2|| ive->vindex2 == v1 && ive->vindex1 == v2)//比较边是否重复
		{
			edli.erase(ive);//如果该边重复，将在边界边池中的这个边删掉
			boo = false;//使当前的边无法加入池
			break;
		}
	}
	if (boo)
	{
		el.vindex1 = v1; 
		el.vindex2 = v2;
		el.edgeface = tf;
		edli.push_back(el);//把当前判断未重复的边加入池
	}
}

//生成savadata3()需要的数据
void edgefind()
{
	cout << "生成边界边" << endl;
	for (TriangularMesh& tm : ttmm)
	{
		for (TriangularFace& tf : tm.triangles)
		{
			//因为三个边所以调用三次savedate3()，在savedata3()判断边界边
			savedate3(tf, tf.vIndex1, tf.vIndex2);
			savedate3(tf, tf.vIndex2, tf.vIndex3);
			savedate3(tf, tf.vIndex3, tf.vIndex1);
		}
		tm.edge = edli;//完成一个面边界边池，加入总边界边池
		edli.clear();//清空当前边界边池以供下一个面使用
	}
	cout << "边界边生成完成" << endl;
}

//生成折线段（提取边界点）
void brokenlinesegment()
{
	cout << "生成折线段" << endl;
	vector<int> vin;
	int x = 0;
	for (TriangularMesh& tm : ttmm)
	{
		vector<Edgeline> ve = tm.edge;
		vector<Edgeline>::iterator ive;
		while (!ve.empty())//边界边池不为空继续循环
		{
			if (vin.empty())//边界点索引数组为空
			{
				ive = ve.begin();//将迭代器指向边界边池头部
				vin.push_back(ive->vindex1);
				x = ive->vindex2;//下面将始终领x为边界边另一个顶点索引
				ve.erase(ive);
				cout << "-------------" << endl;
				cout << vin[0] << "     " << x<<endl;
			}
			else
			{
				if (vin[0] == x)//如果边界点索引数组头部与x相等，判断闭合边界边
				{
					tm.borderVerts.push_back(vin);//将当前边界点索引数组加入边界点索引数组池
					vin.clear();//清空边界点索引数组
					cout << "-------------" << endl;
				}
				else
				{
					for (ive = ve.begin(); ive != ve.end(); ive++)//遍历边界边池
					{
						if (x == ive->vindex1)//判断是否有共同点的边界边
						{
							cout << x << "     ";
							vin.push_back(x);
							x = ive->vindex2;//始终领x为边界边另一个顶点索引
							cout << x <<endl;
							ve.erase(ive);
							break;
						}
						else if (x == ive->vindex2)
						{
							cout << x << "     ";
							vin.push_back(x);
							x = ive->vindex1;//始终领x为边界边另一个顶点索引
							cout << x << endl;
							ve.erase(ive);
							break;
						}
					}
				}
			}
		}
	}
	cout << "折线段生成完成" << endl;
}

//节点向量
void Uniknots(float* knots, int npts, int order)
{
	int i, nknots;
	nknots = npts + order;
	for (i = 0; i < nknots; i++)knots[i] = i;
}

//B样条曲线拟合
GLUnurbsObj* BCurve(float* knot, int nctls, float* ctls, int order)
{
	GLUnurbsObj* oBCurve;
	int nknots = nctls + order;
	oBCurve = gluNewNurbsRenderer();
	gluNurbsProperty(oBCurve, GLU_SAMPLING_TOLERANCE, 1);
	glEnable(GL_MAP1_VERTEX_3);
	gluBeginCurve(oBCurve);
	gluNurbsCurve(oBCurve, nknots, knot, 3, ctls, order, GL_MAP1_VERTEX_3);
	gluEndCurve(oBCurve);
	return oBCurve;
}

//三角网格绘制
void drawTriangularMeshModel()
{
	const vector<TriangularMesh>& meshes = ttmm;
	const vector<Vertex>& verts = vvv;
	int oo = 1,yy=1;
	double a[3];
	a[0] = 0; a[1] = 0; a[2] = 0;
	;
	for (const TriangularMesh& tm : meshes)
	{
		for (const TriangularFace& tf : tm.triangles)
		{
			if (yy == 8) { yy = 1; }//颜色判断代码
			oo = yy;
			a[0] = oo % 2; oo /= 2;
			a[1] = oo % 2; oo /= 2;
			a[2] = oo % 2; yy++;
			glColor3f(a[0], a[1], a[2]);
			Vertex v1 = verts[tf.vIndex1];
			Vertex v2 = verts[tf.vIndex2];
			Vertex v3 = verts[tf.vIndex3];
			Vertex vn1 = tf.vn1;
			Vertex vn2 = tf.vn2;
			Vertex vn3 = tf.vn3;
			
			glBegin(GL_TRIANGLES);
			glNormal3d(vn1.x, vn1.y, vn1.z);
			glVertex3d(v1.x, v1.y, v1.z);
			glNormal3d(vn2.x, vn2.y, vn2.z);
			glVertex3d(v2.x, v2.y, v2.z);
			glNormal3d(vn3.x, vn3.y, vn3.z);
			glVertex3d(v3.x, v3.y, v3.z);
			glEnd();
		}
	}
}

//轮廓绘制
void drawEdgeContourModel()
{
	const vector<TriangularMesh>& meshes = ttmm;
	const vector<Vertex>& verts = vvv;
	;
	for (const TriangularMesh& tm : meshes)
	{
		for (const Edgeline& el : tm.edge)
		{
			Vertex v1 = vvv[el.vindex1];
			Vertex v3 = vvv[el.vindex2];;

			glBegin(GL_LINES);
			glVertex3d(v1.x, v1.y, v1.z);
			glVertex3d(v3.x, v3.y, v3.z);
			glEnd();
		}
	}
}

//B样条绘制
void drawBsplineModel()
{
	GLfloat **ctrlPoints;
	const vector<Vertex>& verts = vvv;
	for (const TriangularMesh& tm : ttmm)
	{
		for (const vector<int>& vertsGroup : tm.borderVerts)
		{
			int cpNum = vertsGroup.size();
			int POWER = 3;
			ctrlPoints = new GLfloat * [cpNum + POWER];
			for (int i = 0; i < cpNum + POWER; i++)
			{
				ctrlPoints[i] = new GLfloat[3];
			}

			for (int i = 0; i < cpNum; i++)
			{
				const Vertex& v = verts[vertsGroup[i]];
				ctrlPoints[i][0] = v.x;
				ctrlPoints[i][1] = v.y;
				ctrlPoints[i][2] = v.z;
			}
			
			for (int i = 0; i < POWER; i++, cpNum++)
			{
				const Vertex& v = verts[vertsGroup[i]];
				ctrlPoints[cpNum][0] = v.x;
				ctrlPoints[cpNum][1] = v.y;
				ctrlPoints[cpNum][2] = v.z;
			}
			
			int kNum = cpNum + POWER + 1;
			GLfloat interval = 1.0 / (kNum - 1);
			GLfloat ui = 0.0;
			GLfloat* knots = new GLfloat[kNum];

			GLUnurbsObj* theNurb = NULL;
			Uniknots(knots, cpNum, POWER + 1);
			theNurb = BCurve(knots, cpNum, *ctrlPoints, POWER + 1);

			for (int i = 0; i < kNum; i++, ui += interval)
			{
				knots[i] = ui;
			}

			gluBeginCurve(theNurb);
			gluNurbsCurve(theNurb, kNum, knots, 3, &ctrlPoints[0][0], 4, GL_MAP1_VERTEX_3);
			gluEndCurve(theNurb);
			
			glBegin(GL_LINE_STRIP);
			
			for (int i = 0; i < cpNum; i++)
			{
				glVertex3f(ctrlPoints[i][0], ctrlPoints[i][1], ctrlPoints[i][2]);
			}
			glEnd();
			
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (x == 1)
	{
		drawTriangularMeshModel();
	}
	else if (x==2)
	{
		drawEdgeContourModel();
	}
	else if (x == 3)
	{
		glColor3f(0.0, 0.0, 1.0);
		drawBsplineModel();
		//glColor3f(1.0, 1.0, 1.0);
		//drawEdgeContourModel();
	}
	glFlush();
}

void Viewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, w / (double)h, 1, 1000);
	glTranslatef(0, 0, -4);
	glScaled(0.015, 0.015, 0.015);
	glRotatef(45, 1, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main()
{
	cout << "开始运行" << endl;
	readxml();          //读取XML文件

	edgefind();         //边界边池生成

	brokenlinesegment();//边界点相连

	for (const TriangularMesh& tm : ttmm)
	{
		for (const Edgeline& el : tm.edge)
		{
			cout <<el.vindex1<<"-----"<<el.vindex2<< endl;
		}
		cout <<"============================"<< endl;
	}

	cout << "输入1生成三角网格" << endl
		<< "输入2生成轮廓的边" << endl
		<< "输入3生成B样条"<<endl;
	cin >> x;
	
	glutInitWindowSize(800, 600);
	glutCreateWindow("sameple");
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	Viewport(0, 0, w, h);
	//主函数
	glutDisplayFunc(display); 
	glutMainLoop();
	
	return 0;
}
