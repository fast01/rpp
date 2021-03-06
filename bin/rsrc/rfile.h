class rfile
{
	void* m_fp;
	
	~rfile()
	{
		close();
	}

	rfile()
	{
		m_fp=null;
	}

	rfile(rstr name,rstr mode="r")
	{
		open(name,mode);
	}

	rbool close()
	{
		if(NULL==m_fp)
		{
			return false;
		}
		if(0!=rff.fclose(m_fp))
		{
			return false;
		}
		m_fp=NULL;
		return true;
	}
	
	//mode: rb只读，wb重新创建写,rb+读写
	//推荐用 r只读，w写，rw读写，本函数只有二进制模式
	rbool open(rstr name,rstr mode="r")
	{
		m_fp=null;
		if(!exist(name))
		{
			if ("rw"==mode)
			{
				if (!create(name))
				{
					return false;
				}
			}
			else
				return false;
		}
		if("r"==mode)
		{
			mode="rb";
		}
		elif("w"==mode)
		{
			mode="wb";
		}
		elif("rw"==mode)
		{
			mode="rb+";
		}
		m_fp=rff.fopen_w(utf16c(name),utf16c(mode));
		if(NULL==m_fp)
		{
			return false;
		}
		return true;
	}

	//强制打开一个新文件
	rbool open_n(rstr name,rstr mode="rw")
	{
		remove(name);
		return open(name,mode);
	}
	
	rbool read(int off,int len,void* buf)
	{
		return read(off.toint8,len,buf);
	}

	rbool read(int8 off,int len,void* buf)
	{
		if(NULL==m_fp)
		{
			return false;
		}
		set_off8(off);
		return len==rff.fread(buf,1,len,m_fp);
	}

	rbool read(int len,void *buf)
	{
		if(NULL==m_fp)
		{
			return false;
		}
		return len==rff.fread(buf,1,len,m_fp);
	}
	
	//读取并返回整个文件，通常用于小文件
	rstr read_all()
	{
		rstr ret;
		ret.set_size(size());
		if (!read(0,size(),ret.begin()))
		{
			return "";
		}
		return ret;
	}

	//读取一行（跳过空行）
	rstr read_line()
	{
		rstr ret;
		uchar c;
		while(read(1,&c))
		{
			if(13==c.toint||10==c.toint)
			{
				if(0==ret.count())
				{
					continue;
				}
				else
				{
					return ret;
				}
			}
			ret+=c;
		}
		return ret;
	}

	rbool write(int off,int len,const void *buf)
	{
		return write(off.toint8,len,buf);
	}

	rbool write(int8 off,int len,const void *buf)
	{
		if(NULL==m_fp)
		{
			return false;
		}
		set_off8(off);
		return len==rff.fwrite(buf,1,len,m_fp);
	}

	rbool write(int len,const void *buf)
	{
		if(NULL==m_fp)
		{
			return false;
		}
		return len==rff.fwrite(buf,1,len,m_fp);
	}

	rbool write(rstr s)
	{
		return write(s.size(),s.begin());
	}

	rbool write_line(rstr s)
	{
		ifn(write(s.size,s.begin))
			return false
		ifn(write(1,"\n"))
			return false
		return true
	}

	int size()
	{
		return size8().toint;
	}

	int8 size8()
	{
		if(NULL==m_fp)
		{
			return 0;//返回-1和返回0各有利弊
		}
		int8 cur=get_off8();
		rff.fseek8(m_fp,0,2);
		int8 ret=get_off8();
		set_off8(cur);
		return ret;
	}

	int get_off()
	{
		return get_off8().toint;
	}

	int8 get_off8()
	{
		if(NULL==m_fp)
		{
			return 0;
		}
		return rff.ftell8(m_fp);
	}

	rbool set_off8(int8 off)
	{
		rff.fseek8(m_fp,off,0);
		return true;
	}

	static rbool exist(rstr name)
	{
		void* fp=rff.fopen_w(utf16c(name),utf16c("rb"));
		if(NULL==fp)
		{
			return false;
		}
		rff.fclose(fp);
		return true;
	}

	//强制创建文件
	static rbool create(rstr name,int8 size=0)
	{
		void* fp=rff.fopen_w(utf16c(name),utf16c("wb"));
		if(NULL==fp)
			return false;
		rff.fclose(fp);
		if(size>0)
		{
			rfile file(name);
			file.set_off8(size-1);
			if(!file.write(1,"\x00"))
			{
				return false;
			}
			return file.close();
		}
		return true;
	}

	static rbool remove(rstr name)
	{
#ifdef _RVM
		return 0==cdecl["_wremove",utf16c(name)];
#else
		return 0==rff.remove_w(utf16c(name))
#endif
	}

	static rbool rename(rstr oldname,rstr newname)
	{
#ifdef _RVM
		return 0==cdecl["_wrename",utf16c(oldname),
			utf16c(newname)];
#else
		return 0==rff.rename_w(utf16c(oldname),utf16c(newname))
#endif
	}
	
	//直接读一个小文件
	static rstr read_all_n(rstr name)
	{
		rfile file(name)
		return file.read_all
	}
	
	static bool write_all_n(rstr name,rstr& s)
	{
		remove(name)
		rfile file(name,'rw')
		return file.write(s)
	}
}