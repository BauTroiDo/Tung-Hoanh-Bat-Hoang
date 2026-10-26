"""Query the CN02-BULK SQLite RVA/type/method index (no decompilation)."""
import argparse,sqlite3
from pathlib import Path

def query(database,assembly=None,type_name=None,method=None,rva=None,limit=20):
    db=Path(database)
    if not db.is_file():raise FileNotFoundError(db)
    if not any((assembly,type_name,method,rva is not None)):
        raise ValueError("Specify at least one selector")
    if not 1<=limit<=100: raise ValueError("limit must be 1..100")
    conditions=[];args=[]
    if assembly:
        conditions.append("assembly = ?");args.append(assembly)
    if type_name:
        conditions.append("type_name LIKE ?");args.append("%"+type_name+"%")
    if method:
        conditions.append("dotnet_signature LIKE ?");args.append("%"+method+"%")
    if rva is not None:
        conditions.append("rva = ?");args.append(rva)
    sql=("SELECT rva, assembly, type_name, dotnet_signature FROM methods WHERE "+
         " AND ".join(conditions)+" ORDER BY assembly,type_name,id LIMIT ?")
    args.append(limit)
    with sqlite3.connect("file:"+db.as_posix()+"?mode=ro",uri=True) as c:
        return c.execute(sql,args).fetchall()

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--database",type=Path,required=True)
    p.add_argument("--assembly")
    p.add_argument("--type",dest="type_name")
    p.add_argument("--method")
    p.add_argument("--rva",type=lambda s:int(s,0))
    p.add_argument("--limit",type=int,default=20)
    a=p.parse_args()
    result=query(a.database,a.assembly,a.type_name,a.method,a.rva,a.limit)
    for rva,image,klass,signature in result:
        print(f"0x{rva:08X} | {image}/{klass} | {signature}")
    print("RESULT_ROWS",len(result))
if __name__=="__main__":main()
