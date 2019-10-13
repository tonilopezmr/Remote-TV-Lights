import './style.css'
import { Component, h, render } from 'preact';
import Header from './view/header'
import WifiPanel from './view/wifi'
import BoardPanel from './view/board'
import { store } from './redux/store'
import { startWs } from './redux/ws'

class App extends Component {

	render() {
		return (
			<div id="app" >
				<Header />
				<BoardPanel />
				<WifiPanel />
			</div>
		)
	}
}

const display = () => {
	console.log(store.getState());
	render(<App />, document.body);
}

store.subscribe(display);

display();
startWs();