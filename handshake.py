import socket
from datetime import datetime
from time import time
from tabulate import tabulate
from rich.table import Table
from rich.console import Console

def handshake(server=False,client=False):
    if not client and client:
        raise f"Server:{client}|Client:{client}"
    
    if server:
        host=socket.gethostname()
        host_ip_addr=socket.gethostbyname(host)
        port=5000
        server_socket=socket.socket(family=socket.AF_INET,type=socket.SOCK_STREAM)
        # socket.AF_INET--> Ipv4 protocols
        # socket.SOCK_STREAM--> TCP Socket
        server_socket.bind((host_ip_addr,port))
        server_socket.listen(2)
        conn,address=server_socket.accept()
        print(f'Hostname:{host}\nHost ip address:{host_ip_addr}')
        while True:
            data=conn.decode()
            if not data:
                break
            print(f'Data received from connected user:{data}')
            conn.send(f'Dit Uoi Datetime:{datetime.strftime("%Y%M%d %H%M%S")}'.encode())
        
        conn.close()
    
    elif client:
        host_ip_addr=input('Enter server ip:')
        host_port=input('Enter server port:')
        if not host_port.isdigit():
            print(f'You did not provide the right type of element({type(host_port)}):{host_port}')
            return
        
        timer1=time()
        client_socket=socket.socket(family=socket.AF_INET,type=socket.SOCK_STREAM)
        timer2=time()

        timer3=time()
        client_socket.connect((host_ip_addr,host_port))
        timer4=time()

        message=input('->')

        while message.upper().strip()!='CC':
            client_socket.send(message.encode())
            data=client_socket.decode()

            print(f'$Received from server:{data}')
            message=input('->')
        
        client_socket.shutdown(True)
        timer5=time()
        client_socket.close()

        console=Console()
        print('Server estimation tool')
        print('*'*10)
        table=Table(title="Raw Timers")
        table.add_column("ID",justify="left",style="blue")
        table.add_column("TIME",justify="left",style="blue")
        table.add_row("Timer1",f'{timer1}')
        table.add_row("Timer2",f'{timer2}')
        table.add_row("Timer3",f'{timer3}')
        table.add_row("Timer4",f'{timer4}')
        table.add_row("Timer5",f'{timer5}')

        table2=Table(title='Intervals')
        table2.add_column('DESCRIPTION',justify='left',style='red')
        table2.add_column('D',justify='left',style='red')


        

if __name__=='__main__':
    handshake()





