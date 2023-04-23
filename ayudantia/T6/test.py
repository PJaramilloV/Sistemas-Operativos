import os

os.chdir('../resultados')
files = os.listdir('.')

result_files = [f for f in files if f.endswith('.json')]
print(result_files)
print("resultados_{}.json".format(len(result_files)))