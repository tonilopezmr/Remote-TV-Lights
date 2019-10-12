import './style.css'
import { Component, h, render } from 'preact';
import Header from './view/header'
import WifiPanel from './view/wifi'
import { store } from './redux/store'

class App extends Component {

	render() {
		return (
			<div id="app" >
				<Header />
				<WifiPanel />
			</div>
		)
	}
}

const display = () => {
	render(<App />, document.body);
}

store.subscribe(display);

display();