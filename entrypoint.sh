/etc/init.d/ssh restart &
netconfd --module=helloworld --no-startup --superuser=pi &
sleep 10
yangcli --server=localhost --user=pi --password=raspberry --run-command="create-subscription" &
cd /node-yuma123
npm test