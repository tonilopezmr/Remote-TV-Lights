import {
    UPDATE_SSID,
    UPDATE_RELAY,
    RECEIVE_CODE
} from './action'

const INITIAL_STATE = {
    boardName: "",
    ssid: "",
    relay: false,
    codes: []
};

export const reducer = (state = INITIAL_STATE, action) => {
    switch (action.type) {
        case UPDATE_RELAY:
            return {
                ...state,
                relay: action.relay,
                boardName: action.boardName
            };
        case UPDATE_SSID:
            return { ...state, ssid: action.ssid };
        case RECEIVE_CODE:
            return { ...state, codes: [...state.codes, action.code] };
        case CLEAN_CODES:
            return { ...state, codes: [] }
        default:
            return state;
    }
};
