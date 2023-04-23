from re import sub
import subprocess
import json
import os

ANS_DIR = 'entregas'
RES_FILE = 'resultados_{}.json'
RES_DIR = 'resultados'
SUC_DIR = 'exitos' 

def change_names() -> None:
    root = os.getcwd()
    for d in os.listdir('.'):
        if d == "__MACOSX": continue
        name_list = d.split("__")
        name = name_list[0].split("_")[0] + '_' + name_list[1].split("_")[0]
        os.rename(root + '/' + d, root + '/' + name)

def check_format(*args) -> None:
    root = os.getcwd()
    for d in os.listdir('.'):
        if d == "__MACOSX": continue
        if os.path.isdir(root + "/" + d):
            # args = (lista_archivos, )     f = lista_archivos     f[0] = archivo1
            #for f in args:
            #    print(f, args, os.listdir(root + "/" + d))
            #    print(os.listdir(root + "/" + d), args[0],args[0][0] not in os.listdir(root + "/" + d))
            #    walk_root, walk_subdirs = os.walk(root + "/" + d)
            #    print(walk_root, walk_subdirs)
            #    if f[0] not in os.listdir(root + "/" + d):
            #        raise Exception(f"Falta el archivo '{f[0]}' en la carpeta '{d}'")
            for f_list in args:
                for f in f_list:
                    if not check_format_loop(f, root + "/" + d):
                        raise Exception(f"Falta el archivo '{f}' en la carpeta '{d}'")

def check_format_loop(file: str, dir: str) -> bool:
    sliced = file.split('/')
    file_relative_path = ''
    if len(sliced) > 1:
        file = sliced[-1]
        file_relative_path = '/'.join(sliced[:-1])

    for walk_path, _, walk_files in os.walk(dir):
        if file_relative_path not in walk_path: continue
        if file in walk_files:
            return True
    return False



def check_all(main_path: str, project_path: str, result_folder: str, result_file: str, source_file: str, success_folder: str) -> None:
    root = os.getcwd()
    ans = {'results': []}
    for d in os.listdir('.'):
        if d == "__MACOSX": continue
        if os.path.isdir(root + "/" + d):
            subprocess.run(["cp", f"{root}/{d}/{source_file}", f"{main_path}/{source_file}"])
            os.chdir(main_path)
            name = d.replace('_', ' ')
            curr = {'name': name.lower()}
            print(f"\r - {name}: eval", end='')
            try:
                p = subprocess.run(['make', 'eval'], capture_output=True, timeout=60*5)
            except subprocess.TimeoutExpired:
                curr['result'] = 'timeout'
                curr['error'] = p.stderr.decode('utf-8')
            if p.returncode == 0:
                curr['result'] = 'ok'
                curr['error'] = p.stderr.decode('utf-8')
            else:
                curr['result'] = 'error'
                curr['error'] = p.stderr.decode('utf-8')

            print(f"\r - {name}: clean", end='')
            subprocess.run(["make", "clean"], capture_output=True)
            subprocess.run(["cp", f"{main_path}/resultados.txt", f"{project_path}/Resultados/{d}.txt"])

            if curr['result'] == 'ok':
                subprocess.run(["cp", '-R', f"{root}/{d}", f"{project_path}/{success_folder}/{d}"])
                subprocess.run(["cp", f"{main_path}/resultados.txt", f"{project_path}/{success_folder}/{d}.txt"])
                subprocess.run(['rm', '-rf', f"{root}/{d}"])
                print(f"\r - {name}: revisado con exito")
            else:
                print(f"\r - {name}: revisado")
            
            ans["results"].append(curr)
            with open(f'../{result_folder}/{result_file}', 'w') as f:
                json.dump(ans, f)
            
            os.chdir(root)
    

def main():
    """Funcion principal"""
    principal = os.getcwd()
    os.chdir('..')
    project = os.getcwd()
    os.chdir(ANS_DIR)

    while True:
        print("¿Que desea hacer?")
        print("1. Chequear archivos de entregas")
        print("2. Formatear carpeta de entregas")
        print("3. Revisar entregas")
        print("0. Salir")
        choice = int( input("Ingrese opcion: ") )
        if choice == 0:
            break
        elif choice == 1:
            files = input("Ingrese los archivos necesarios para la entrega (separados por espacios): ").split(' ')
            check_format(files)
        elif choice == 2:
            change_names()
        elif choice == 3:
            target = input("Ingrese el nombre del archivo principal: ")
            print('Revisando...')
            files = os.listdir('../{}'.format(RES_DIR))
            results = [f for f in files if f.endswith('.json')]
            result_f = RES_FILE.format(len(results))
            check_all(principal, project, RES_DIR, result_f, target, SUC_DIR)


if __name__ == "__main__":
    main()


