// This example program is created by thecplusplusuy for demonstration purposes. It's a simple 3D model loader (wavefront (.obj)), which is capable to load materials and UV textures:
// http://www.youtube.com/user/thecplusplusguy
// Free source, modify if you want, LGPL licence (I guess), I would be happy, if you would not delete the link
// so other people can see the tutorial
// this file is the objloader.cpp
// Modified for migration to SDL2/Linux by SandUhrGucker

#include "objloader.h"

coordinate::coordinate(float a,float b,float c) {
    x = a;
    y = b;
    z = c;
}

face::face(int facen,int f1,int f2,int f3,int t1,int t2,int t3,int m) {
    facenum = facen;
    faces[0] = f1;
    faces[1] = f2;
    faces[2] = f3;
    texcoord[0] = t1;
    texcoord[1] = t2;
    texcoord[2] = t3;
    mat = m;
    four = false;
}

face::face(int facen,int f1,int f2,int f3,int f4,int t1,int t2,int t3,int t4,int m) {
    facenum = facen;
    faces[0] = f1;
    faces[1] = f2;
    faces[2] = f3;
    faces[3] = f4;
    texcoord[0] = t1;
    texcoord[1] = t2;
    texcoord[2] = t3;
    texcoord[3] = t4;
    mat = m;
    four = true;
}
	
material::material(const char* na,float al,float n,float ni2,float* d,float* a,float* s,int i,int t) {
    name = na;
    alpha = al;
    ni = ni2;
    ns = n;
    dif[0] = d[0];
    dif[1] = d[1];
    dif[2] = d[2];
		
    amb[0] = a[0];
    amb[1] = a[1];
    amb[2] = a[2];
		
    spec[0] = s[0];
    spec[1] = s[1];
    spec[2] = s[2];
		
    illum = i;
    textureID = t;
}
	
texcoord::texcoord(float a,float b) {
    u = a;
    v = b;
}

int objloader::load(const char* filename) {
	using namespace std;
	std::ifstream in(filename);	// open the model file

	if (!in.is_open()) {
		std::cerr << "ERROR: Can't open: " << filename << std::endl;
		return -1;
	}
    else {
        std::cout << "NOTE: Opened: " << filename << std::endl;
    }   

	char buf[256];	// temp buffer
	int currentMaterial=0;	// the current (default) material is 0, it's used, when we read the faces

	while (!in.eof()) {
		in.getline(buf,256);	// while we are not in the end of the file, read everything as a string to the coord vector
		coord.push_back(new std::string(buf));
	}

	for (int i=0;i<coord.size();i++) { // and then go through all line and decide what kind of line it is
		if ((*coord[i])[0]=='#') { // if it's a comment
			continue;	// we don't have to do anything with it
        }   
		else if ((*coord[i])[0]=='v' && (*coord[i])[1]==' ') {	// if a vertex
			float tmpx,tmpy,tmpz;
			sscanf(coord[i]->c_str(),"v %f %f %f",&tmpx,&tmpy,&tmpz);	// read the 3 floats, which makes up the vertex
			vertex.push_back(new coordinate(tmpx,tmpy,tmpz));	// and put it in the vertex vector
		}
        else if ((*coord[i])[0]=='v' && (*coord[i])[1]=='n') {	// if it's a normal vector
			float tmpx,tmpy,tmpz;
			sscanf(coord[i]->c_str(),"vn %f %f %f",&tmpx,&tmpy,&tmpz);
			normals.push_back(new coordinate(tmpx,tmpy,tmpz));	// basically do the same
			isNormals=true;
		}
        else if ((*coord[i])[0]=='f') {	// if it's a face
			int a,b,c,d,e;

			if (count(coord[i]->begin(),coord[i]->end(),' ')==4){	// if this is a quad
				if (coord[i]->find("// ")!=std::string::npos) {	// if it's contain a normal vector, but not contain texture coorinate
					sscanf(coord[i]->c_str(),"f %d//%d %d//%d %d//%d %d//%d",&a,&b,&c,&b,&d,&b,&e,&b);	// read in this form
					faces.push_back(new face(b,a,c,d,e,0,0,0,0,currentMaterial));	// and put to the faces, we don't care about the texture coorinate in this case
                    // and if there is no material, it doesn't matter, what is currentMaterial
				}
                else if (coord[i]->find("/")!=std::string::npos) {	// if we have texture coordinate and normal vectors
					int t[4];	// texture coordinates
					// read in this form, and put to the end of the vector
					sscanf(coord[i]->c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&a,&t[0],&b,&c,&t[1],&b,&d,&t[2],&b,&e,&t[3],&b);
					faces.push_back(new face(b,a,c,d,e,t[0],t[1],t[2],t[3],currentMaterial));
				}
                else {
					// else we don't have normal vectors nor texture coordinate
					sscanf(coord[i]->c_str(),"f %d %d %d %d",&a,&b,&c,&d);
					faces.push_back(new face(-1,a,b,c,d,0,0,0,0,currentMaterial));		
				}
			}
            else {	// if it's a triangle
                // do the same, except we use one less vertex/texture coorinate/face number
                if (coord[i]->find("//")!=std::string::npos) {
                    sscanf(coord[i]->c_str(),"f %d//%d %d//%d %d//%d",&a,&b,&c,&b,&d,&b);
                    faces.push_back(new face(b,a,c,d,0,0,0,currentMaterial));
                }
                else if (coord[i]->find("/")!=std::string::npos) {
                    int t[3];
                    sscanf(coord[i]->c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d",&a,&t[0],&b,&c,&t[1],&b,&d,&t[2],&b);
                    faces.push_back(new face(b,a,c,d,t[0],t[1],t[2],currentMaterial));
                }
                else {
                    sscanf(coord[i]->c_str(),"f %d %d %d",&a,&b,&c);
                    faces.push_back(new face(-1,a,b,c,0,0,0,currentMaterial));					
                }
			}
        }
        else if ((*coord[i])[0]=='u' && (*coord[i])[1]=='s' && (*coord[i])[2]=='e') {	// use material material_name
            char tmp[200];

            sscanf(coord[i]->c_str(),"usemtl %s",tmp);	// read the name of the material to tmp
            for (int i=0;i<materials.size();i++) {	// go through all of the materials
                if (strcmp(materials[i]->name.c_str(),tmp)==0) {	// and compare the tmp with the name of the material
                    currentMaterial = i;	// if it's equal then set the current material to that
                    break;
                }
            }
        }
        else if ((*coord[i])[0]=='m' && (*coord[i])[1]=='t' && (*coord[i])[2]=='l' && (*coord[i])[3]=='l') {
            // material library, a file, which contain all of the materials
            char filen[200];

            sscanf(coord[i]->c_str(),"mtllib %s",filen);	// read the filename
            std::ifstream mtlin(filen);	// open the file

            if (!mtlin.is_open()) {	// if not opened error message, clean all memory, return with -1
                std::cerr << "ERROR: Can't open material file: " << filen << std::endl;
                clean();
                return -1;
            }
            else {
                std::cout << "NOTE: Opened: " << filen << std::endl;
            }   

            isMaterial=true;	// we use materials

            std::vector<std::string> tmp;// contain all of the line of the file
            char c[200];
            while(!mtlin.eof()) {
                mtlin.getline(c,200);	// read all lines to tmp
                tmp.push_back(c);
            }

            char name[200];	// name of the material
            char filename[200];	// filename of the texture
            float amb[3],dif[3],spec[3],alpha=0,ns=0,ni=0;	// colors, shininess, and something else
            int illum=0;
            uint textureID=-1;
            bool ismat=false;	// do we already have a material read in to these variables?

            strcpy(filename,"\0");	// set filename to nullbyte character

            for (int i=0;i<tmp.size();i++) { // go through all lines of the mtllib file
                if (tmp[i][0]=='#') {	// we don't care about comments
                    continue;
                }

                if (tmp[i][0]=='n' && tmp[i][1]=='e' && tmp[i][2]=='w') {	// new material
                    if (ismat) {	// if we have a material
                        if(strcmp(filename,"\0")!=0) {	// if we have a texture
                            materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,textureID));	// push back
                            strcpy(filename,"\0");
                        }
                        else {
							materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,-1));		// push back, but use -1 to texture		
                        }
                    }

                    ismat = false;	// we start from a fresh material
                    sscanf(tmp[i].c_str(),"newmtl %s",name);	// read in the name
                }
                else if (tmp[i][0]=='N' && tmp[i][1]=='s') {	// the shininess
                    sscanf(tmp[i].c_str(),"Ns %f",&ns);
                    ismat = true;
                }
                else if (tmp[i][0]=='K' && tmp[i][1]=='a') {	// the ambient
                    sscanf(tmp[i].c_str(),"Ka %f %f %f",&amb[0],&amb[1],&amb[2]);
                    ismat = true;
                }
                else if (tmp[i][0]=='K' && tmp[i][1]=='d') {	// the diffuse
                    sscanf(tmp[i].c_str(),"Kd %f %f %f",&dif[0],&dif[1],&dif[2]);
                    ismat = true;
                }
                else if (tmp[i][0]=='K' && tmp[i][1]=='s') {	// the specular
                    sscanf(tmp[i].c_str(),"Ks %f %f %f",&spec[0],&spec[1],&spec[2]);
                    ismat = true;
                }
                else if (tmp[i][0]=='N' && tmp[i][1]=='i') {	// the I don't know what is this
                    sscanf(tmp[i].c_str(),"Ni %f",&ni);
                    ismat = true;
                }
                else if (tmp[i][0]=='d' && tmp[i][1]==' ') {	// the alpha
                    sscanf(tmp[i].c_str(),"d %f",&alpha);
                    ismat = true;
                }
                else if (tmp[i][0]=='i' && tmp[i][1]=='l') {	// the illum (don't ask)
                    sscanf(tmp[i].c_str(),"illum %d",&illum);
                    ismat = true;
                }
                else if (tmp[i][0]=='m' && tmp[i][1]=='a') {	// and the texture
                    sscanf(tmp[i].c_str(),"map_Kd %s",filename);

                    if (filename[0] != '\0') { //  could be a different map_*
                        textureID = loadTexture(filename);
                        ismat = true;
                    }
                }
            }

            if (ismat) { // there is no newmat after the last newmat, so we have to put the last material 'manually'
                if (strcmp(filename,"\0")!=0) {
                    materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,textureID));
                }
                else {
					materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,-1));				
                }
            }
        }
        else if ((*coord[i])[0]=='v' && (*coord[i])[1]=='t') {	// back to the obj file, texture coorinate
            float u,v;

            sscanf(coord[i]->c_str(),"vt %f %f",&u,&v);	// read the uv coordinate
            textureCoordinate.push_back(new texcoord(u,1-v));	// push back 1-v instead of normal v, because obj file use the upper left corner as 0,0 coordinate
            // but OpenGL use bottom left corner as 0,0, so convert it
            isTexture = true;
        }
    }

    isMaterial = materials.size() > 0;

	std::cout << vertex.size() << " " << normals.size() << " " << faces.size() << " " << materials.size() << std::endl;

	// draw
	int num;
	num = glGenLists(1);	// generate a unique identifier for the list
	glNewList(num,GL_COMPILE);
	int last=-1;	//  the last material (default -1, which doesn't exist, so we use the first material)

	for (int i=0;i<faces.size();i++) {	// go throught all faces
        struct face *face=faces[i];

		if (last!=face->mat && isMaterial) {	// if we have a material AND the last material is not the same
			// set all of the material property
			float diffuse[]={materials[face->mat]->dif[0],materials[face->mat]->dif[1],materials[face->mat]->dif[2],1.0};
			float ambient[]={materials[face->mat]->amb[0],materials[face->mat]->amb[1],materials[face->mat]->amb[2],1.0};
			float specular[]={materials[face->mat]->spec[0],materials[face->mat]->spec[1],materials[face->mat]->spec[2],1.0};
			glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
			glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
			glMaterialf(GL_FRONT,GL_SHININESS,materials[face->mat]->ns);
			last=face->mat;	// set the current to last
			if(materials[face->mat]->textureID==-1)	// if we don't have texture, disable it, else enable it
				glDisable(GL_TEXTURE_2D);
			else{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,materials[face->mat]->textureID);	// and use it
			}
		}

		if (face->four) {
			glBegin(GL_QUADS);

            if (isNormals) {
                glNormal3f(normals[face->facenum-1]->x,normals[face->facenum-1]->y,normals[face->facenum-1]->z);
            }
					
            long idx;
            long fidx;

            idx = 0;
            if (isTexture && (materials[face->mat]->textureID != -1)) {
                idx = face->texcoord[0]-1;

                glTexCoord2f(textureCoordinate[idx]->u,textureCoordinate[idx]->v);
            }
				
            fidx = face->faces[0]-1;
            if (fidx > -1) {
                glVertex3f(vertex[fidx]->x,vertex[fidx]->y,vertex[fidx]->z);
            }
				
            idx = 0;
            if (isTexture && (materials[face->mat]->textureID != -1)) {
                idx = face->texcoord[1]-1;

                glTexCoord2f(textureCoordinate[idx]->u,textureCoordinate[idx]->v);
            }
				
            fidx = face->faces[1]-1;
            if (fidx > -1) {
                glVertex3f(vertex[fidx]->x,vertex[fidx]->y,vertex[fidx]->z);
            }
				
            idx = 0;
            if (isTexture && (materials[face->mat]->textureID != -1)) {
                idx = face->texcoord[2]-1;

                glTexCoord2f(textureCoordinate[idx]->u,textureCoordinate[idx]->v);
            }
				
            fidx = face->faces[2]-1;
            if (fidx > -1) {
                glVertex3f(vertex[fidx]->x,vertex[fidx]->y,vertex[fidx]->z);
            }   
				
            idx = 0;
            if (isTexture && (materials[face->mat]->textureID != -1)) {
                idx = face->texcoord[3]-1;

                if (idx > -1) {
                    glTexCoord2f(textureCoordinate[idx]->u,textureCoordinate[idx]->v);
                }   
            }   
				
            fidx = face->faces[3]-1;
            if (fidx > -1) {
                glVertex3f(vertex[fidx]->x,vertex[fidx]->y,vertex[fidx]->z);
            }   

			glEnd();
		}
        else {
			glBegin(GL_TRIANGLES);

            if (isNormals) {
                glNormal3f(normals[face->facenum-1]->x,normals[face->facenum-1]->y,normals[face->facenum-1]->z);
            }

            if (isTexture && materials[face->mat]->textureID!=-1) {
                glTexCoord2f(textureCoordinate[face->texcoord[0]-1]->u,textureCoordinate[face->texcoord[0]-1]->v);
            }

            glVertex3f(vertex[face->faces[0]-1]->x,vertex[face->faces[0]-1]->y,vertex[face->faces[0]-1]->z);
				
            if (isTexture && materials[face->mat]->textureID!=-1) {
                glTexCoord2f(textureCoordinate[face->texcoord[1]-1]->u,textureCoordinate[face->texcoord[1]-1]->v);
            }
				
            glVertex3f(vertex[face->faces[1]-1]->x,vertex[face->faces[1]-1]->y,vertex[face->faces[1]-1]->z);
				
            if (isTexture && materials[face->mat]->textureID!=-1) {
                glTexCoord2f(textureCoordinate[face->texcoord[2]-1]->u,textureCoordinate[face->texcoord[2]-1]->v);
            }
				
            glVertex3f(vertex[face->faces[2]-1]->x,vertex[face->faces[2]-1]->y,vertex[face->faces[2]-1]->z);
			glEnd();
		}
	}

	glEndList();
	clean();
	lists.push_back(num);

	return num;
}

void objloader::clean() {
	// delete all the dynamically allocated memory
	for(int i=0;i<coord.size();i++)
		delete coord[i];
	for(int i=0;i<faces.size();i++)
		delete faces[i];
	for(int i=0;i<normals.size();i++)
		delete normals[i];
	for(int i=0;i<vertex.size();i++)
		delete vertex[i];
	for(int i=0;i<materials.size();i++)
		delete materials[i];
	for(int i=0;i<textureCoordinate.size();i++)
		delete textureCoordinate[i];
	// and all elements from the vector
	coord.clear();
	faces.clear();
	normals.clear();
	vertex.clear();
	materials.clear();
	textureCoordinate.clear();
}

objloader::~objloader() {
	// delete lists and textures
	for (std::vector<unsigned int>::const_iterator it=textureIDs.begin();it!=textureIDs.end();it++){
		glDeleteTextures(1,&(*it));
	}

	for (std::vector<unsigned int>::const_iterator it=lists.begin();it!=lists.end();it++) {
		glDeleteLists(*it,1);
	}
}

// load the filename textures (only BMP, R5G6B5 format)
unsigned int objloader::loadTexture(const char* filename) {
	unsigned int textureID;
	glGenTextures(1,&textureID);
	SDL_Surface* img=SDL_LoadBMP(filename);

    if (!img) {
        std::cerr << "ERROR: SDL: " << filename << ": " << SDL_GetError() << std::endl;
    }
    else {
        std::cout << "NOTE: Opened: " << filename << std::endl;
    }

	glBindTexture(GL_TEXTURE_2D,textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,img->pixels);
	glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);	// maybe just this
	SDL_FreeSurface(img);
	textureIDs.push_back(textureID);

	return textureID;
}

objloader::objloader(){
	// at default we set all booleans to false, so we don't use anything
	isMaterial = false;
	isNormals = false;
	isTexture = false;
}
