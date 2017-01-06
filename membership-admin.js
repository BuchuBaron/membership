Drupal.behaviors.membership_admin = function(context) {
  // Set up click events on the display selector checkboxes.
  $.each(['status', 'sort', 'identity', 'contact', 'emergency'], function(index, name) {
    $('#' + name, context).click(function() {
      if ($(this).attr('checked')) {
        setSessionVariable(name, 1, function() {
          $('.' + name).show();
        });
      }
      else {
        setSessionVariable(name, 0, function() {
          $('.' + name).hide();
        });
      }
    });
  });

  // Initialize the display checkboxes.
  $.each(['status', 'sort', 'identity', 'contact', 'emergency'], function(index, name) {
    getSessionVariable(name, function(val) {
      // The function was successful -> a Session value had been set before.
      if (val == 1) {
        $('.' + name).show();
        $('#' + name).attr('checked', 'checked');
      }
      else {
        $('.' + name).hide();
        $('#' + name).removeAttr('checked');
      }
    });
  });

  // Set up select-all
  $('#select-all', context).click(function() {
    if ($(this).attr('checked')) {
      $('.selection').attr('checked', 'checked');
    }
    else {
      $('.selection').removeAttr('checked');
    }
    checkActionDropdownState();
  });

  // Set up unselect all when one checkbox is unselected.
  $('.selection', context).click(function() {
    if (!$(this).attr('checked')) {
      $('#select-all').removeAttr('checked');
    }
    checkActionDropdownState();
  });

  // Disable the 'Action' dropdown if there is no selection.
  checkActionDropdownState();

  // Set up the actions on selected records.
  $('#run-action', context).click(function() {
    var command = $('#action').val();
    // Assemble a list of ids to send to the server with the command:
    var selected_members = $.map($('.selection:checked', context), function(element) {
      return element.value;
    });
    var post_data = {
      selected_members: selected_members.join(',')
    };

    $.post('/membership/update/' + command, post_data, function(result) {
      eval("output=" + result);
      if (output.status) {
        // Update the changed records in situ, or refresh the page.
        $(output.data).each(function(idx, val) {
          if (command == 'paid') {
            $('TD#paid-' + val).html(1);
          }
          else if (command == 'unpaid') {
            $('TD#paid-' + val).html(0);
          }
          else if (command == 'active') {
            $('TD#status-' + val).html('active');
          }
          else if (command == 'rejected') {
            $('TD#status-' + val).html('rejected');
          }
          else if (command == 'expired') {
            $('TD#status-' + val).html('expired');
          }
          else if (command == 'export') {
            // Output.data.val is the path of the download csv
            if (val.path) {
              $('iframe#dliframe').attr('src', '/membership/download?path=' + val.path);
            }
            else if (val.error) {
              alert(val.error);
            }
            else {
              alert('Something went wrong');
            }
          }
        });
      }
      else {
        alert("Failed " + command + " to get " + output.status + " with " + output.data);
      }
    });
  });

  // Activate the run button when a valid action has been chosen
  $('#action', context).change(function() {
    if ($(this).val() == 'action') {
      $('#run-action').attr('disabled', true).attr('title', Drupal.t('Choose an action to run first.'));
    }
    else {
      $('#run-action').attr('disabled', false).attr('title', Drupal.t('Click here to run the chosen action against all the checked records below.'));
    }
  });
}

function checkActionDropdownState() {
  $('#action').attr('title', Drupal.t('Select the records you want to modify by checking their checkboxes.'));
  $('#action').attr('disabled', true);

  var selectionCount = 0;
  $('.selection').each(function() {
    if ($(this).attr('checked')) {
      $('#action').attr('disabled', false);
      selectionCount += 1;
      $('#selected-count').html(selectionCount);
    }
  });

  $('#action:enabled').attr('title', Drupal.t('Choose your action then click the Run button.'));
  if ($('#action:enabled').size() && $('#action').val() != 'action') {
    $('#run-action').attr('disabled', false).attr('title', Drupal.t('Click here to run the chosen action against all the checked records below.'));
  }
  else {
    $('#run-action').attr('disabled', true).attr('title', Drupal.t('Choose an action to run first.'));
  }
}

function setSessionVariable(name, value, callback) {
  var post_data = {
    name: value
  };

  $.ajax({
    type: 'POST',
    data: name + '=' + value,
    url: Drupal.settings.basePath + 'admin/ajax/set/' + name,
    dataType: 'json',
    success: callback,
  });
}

function getSessionVariable(name, callback) {
  $.ajax({
    type: 'POST',
    url: Drupal.settings.basePath + 'admin/ajax/get/' + name,
    dataType: 'json',
    success: function(result) {
      if (result.status) {
        callback(result.data)
      }
    },
  });
}
