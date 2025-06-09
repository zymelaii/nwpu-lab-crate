import argparse
from pypdf import PdfWriter
from docx2pdf import convert as docx2pdf
import pypandoc
import os

def dirpath(dir: str = '.') -> str:
    return os.path.normpath(os.path.join(os.path.dirname(__file__), dir))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--uid', '-u', type=str, required=True, help='specify id of the student')
    parser.add_argument('--name', '-n', type=str, required=True, help='specify name of the student')
    args = parser.parse_args()

    root_dir = dirpath()
    output_dir = dirpath('dist')
    os.makedirs(output_dir, exist_ok=True)

    for i in range(4):
        docx2pdf(f'{root_dir}/cover-task{i + 1}.docx', f'{output_dir}/cover-task{i + 1}.pdf', keep_active=True)
        if not os.path.exists(f'{output_dir}/task{i + 1}.pdf'):
            print(f'error: {output_dir}/task{i + 1}.pdf not found')
            continue
        merger = PdfWriter()
        for pdf in [f'{output_dir}/cover-task{i + 1}.pdf', f'{output_dir}/task{i + 1}.pdf']:
            merger.append(pdf)
        merger.write(f'{output_dir}/{args.uid}-{args.name}-task{i + 1}.pdf')
        merger.close()
