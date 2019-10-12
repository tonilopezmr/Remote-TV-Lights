import { Component, h, render } from 'preact';


export default class WifiPanel extends Component {

    constructor(props) {
        super(props);
        this.state = {
            ssid: "",
            password: "",
            passwordError: false,
            disableSaveButton: true
        }
    }

    onSaveClick() {
        //console.log(this.state.ssid + this.state.password);
    }

    validPassword(password) {
        return password.length < 8;
    }

    ssidChange(event) {
        this.setState({ ...this.state, ssid: event.target.value });
    }

    passwordChange(event) {
        const password = event.target.value;
        const error = this.validPassword(password);
        this.setState({
            ...this.state,
            password: password,
            passwordError: error,
            disableSaveButton: error
        });
    }

    render(props, state) {
        return (
            <section class="section">
                <div style="max-width: 350px;" class="container">
                    <div class="field is-centered">
                        <h1 style="text-align: center; margin-bottom: 25px;" class="title">Wifi</h1>
                    </div>
                    <div class="field">
                        <div class="control">
                            <input onInput={this.ssidChange.bind(this)}
                                class="input" type="text" placeholder="SSID" />
                        </div>
                    </div>
                    <div class="field">
                        <div class="control">
                            <input onInput={this.passwordChange.bind(this)}
                                className={state.passwordError ? "input is-danger" : "input"} type="password" placeholder="Password" />
                        </div>

                        {state.passwordError ? <p class="help is-danger">This email is invalid</p> : ""}
                    </div>
                    <a onClick={this.onSaveClick.bind(this)}
                        class="button is-primary is-fullwidth" disabled={state.disableSaveButton}>Save</a>
                </div>
            </section>
        )
    }
}